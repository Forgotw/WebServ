package main

import (
	"flag"
	"fmt"
	"io"
	"math/rand"
	"net/http"
	"strings"
	"sync/atomic"
	"time"

	"github.com/schollz/progressbar/v3"
)

const (
	NUMROUTINE = 20
	CHARSET    = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
)
const (
	BOLD  = "\033[1m"
	RED   = "\033[31m"
	CYAN  = "\033[36m"
	RESET = "\033[0m"
)

type Config struct {
	Verbose     bool
	IP          string
	Port        string
	PayloadSize int
	Duration    int
}

var config Config
var client = &http.Client{
	Transport: &http.Transport{
		MaxIdleConnsPerHost: NUMROUTINE,
	},
}
var (
	totalRequest int64 = 0
	totalError   int64 = 0
)

func main() {
	payload := stringWithCharset()
	endpoint := "http://" + config.IP + ":" + config.Port + "/" + payload
	done := make(chan bool)
	bar := createProgressBar()
	startTime := time.Now()
	for i := 0; i < NUMROUTINE; i++ {
		go sendRequest(endpoint, done)
	}
	go updateProgressBar(done, startTime, bar)
	time.Sleep(time.Duration(config.Duration) * time.Second)
	close(done)
	showResult()
}

func init() {
	flag.BoolVar(&config.Verbose, "v", false, "Show response of each request")
	flag.StringVar(&config.IP, "ip", "localhost", "IP to send requests to")
	flag.StringVar(&config.Port, "port", "8080", "Port to send requests to")
	flag.IntVar(&config.PayloadSize, "size", 0, "Size of the payload (default 0)")
	flag.IntVar(&config.Duration, "duration", 30, "Duration of the test in seconds")
	flag.Parse()

	fmt.Printf("%s", CYAN)
	fmt.Printf("Run a %v secondes stress test on %s with a payload of size %s.\n",
		fmt.Sprintf("%s%v%s", RESET+BOLD, config.Duration, RESET+CYAN),
		fmt.Sprintf("%shttp://%v:%v%s", RESET+BOLD, config.IP, config.Port, RESET+CYAN),
		fmt.Sprintf("%s%d%s", RESET+BOLD, config.PayloadSize, RESET+CYAN),
	)
	fmt.Printf("%s", RESET)
}

func stringWithCharset() string {
	b := make([]byte, config.PayloadSize)
	for i := range b {
		b[i] = CHARSET[rand.Intn(len(CHARSET))]
	}
	return string(b)
}

func createProgressBar() *progressbar.ProgressBar {
	return progressbar.NewOptions64(int64(config.Duration),
		progressbar.OptionEnableColorCodes(true),
		progressbar.OptionFullWidth(),
		progressbar.OptionSetDescription("[cyan]Request/seconde:[reset] [bold]0[reset] |"),
		progressbar.OptionClearOnFinish(),
		progressbar.OptionSetTheme(progressbar.Theme{
			Saucer:        "[green]=[reset]",
			SaucerHead:    "[green]>[reset]",
			SaucerPadding: " ",
			BarStart:      "[",
			BarEnd:        "]",
		}))
}

func updateProgressBar(done chan bool, startTime time.Time, bar *progressbar.ProgressBar) {
	for {
		requestsPerSecond := float64(atomic.LoadInt64(&totalRequest)) / float64(time.Since(startTime).Seconds())
		select {
		case <-done:
			return
		default:
			bar.Describe("[cyan]Request/seconde:[reset] [bold]" + fmt.Sprintf("%.0f", requestsPerSecond) + "[reset] |")
			bar.Add(1)
			time.Sleep(1 * time.Second)
		}
	}
}

func sendRequest(url string, done chan bool) {
	for {
		select {
		case <-done:
			return
		default:
			resp, err := client.Get(url)
			if err != nil {
				splitMsg := strings.Split(err.Error(), "\": ")
				fmt.Println(RED + BOLD + "Error: " + splitMsg[1] + RESET)
				totalError++
				continue
			}
			atomic.AddInt64(&totalRequest, 1)
			if config.Verbose {
				body, err := io.ReadAll(resp.Body)
				if err != nil {
					resp.Body.Close()
					continue
				}
				fmt.Println(string(body))
			}
			resp.Body.Close()
		}
	}
}

func showResult() {
	fmt.Printf("%s", CYAN)
	fmt.Printf("A total of %v (%s) requests were sent, at an average of %s requests/second.\n",
		fmt.Sprintf("%s%v%s", RESET+BOLD, totalRequest, RESET+CYAN),
		fmt.Sprintf("%s%v%s", RESET+BOLD+RED, totalError, RESET+CYAN),
		fmt.Sprintf("%s%.0f%s", RESET+BOLD, float64(totalRequest)/float64(config.Duration), RESET+CYAN),
	)
	fmt.Printf("%s", RESET)
}
