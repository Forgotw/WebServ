# **WebServ**

**WebServ** is a fully custom-built HTTP server developed as part of the École 42 curriculum. This project focuses on understanding how web servers function by building one from scratch, handling HTTP requests, managing responses, and supporting CGI (Common Gateway Interface), including FastCGI.

---

## **Overview**

WebServ is a lightweight and efficient HTTP server that complies with the HTTP/1.1 protocol. It supports:
- Parsing configuration files to define server behavior.
- Serving static and dynamic content.
- Managing HTTP methods and status codes.
- Handling CGI scripts (including FastCGI via sockets).

This project emphasizes low-level programming concepts, including socket programming, process management, and inter-process communication.

---

## **Features**

### **Mandatory Features**
- **Static Content Delivery**:
  - Serve static files such as HTML, CSS, JavaScript, and images.
  
- **Dynamic Content via CGI**:
  - Execute CGI scripts (e.g., Python, PHP, Perl) to generate dynamic content.

- **HTTP Methods**:
  - Full support for `GET`, `POST`, and `DELETE` methods.

- **HTTP Status Codes**:
  - Correct handling of status codes (e.g., `200 OK`, `404 Not Found`, `500 Internal Server Error`).

- **Configuration Parsing**:
  - Define server behavior in a configuration file, including:
    - Port and host settings.
    - Server root and index files.
    - Error pages.

- **Chunked Transfer-Encoding**:
  - Proper handling of chunked requests and responses.

### **Bonus Features**
- **Full CGI Support**:
  - Execute any script via CGI, adhering to the Common Gateway Interface standards.
  
- **FastCGI Integration**:
  - Communication with FastCGI applications using a dedicated socket for high-performance dynamic content generation.

- **Multiple Server Instances**:
  - Support for running multiple servers on different ports simultaneously.

- **Request Routing**:
  - Route requests based on the URL path to specific resources or scripts.

- **Custom Error Pages**:
  - Serve user-defined error pages for specific HTTP status codes.

- **Concurrency**:
  - Handle multiple simultaneous connections using select/poll/epoll (platform-dependent).

---

## **How It Works**

1. **Configuration File**:
   - WebServ uses a configuration file to define its behavior. Below is an example configuration:

     ```config
      server { # simple load balancing
      	host			127.0.0.1;
      	listen			8080;
      	error_page		400 401 403 404 405 411 413 431 500 501;
      	error_dir		www/fancy_error/;
      	access_log		logs/iplocal.access.log;
          upload          www/upload/;
      
      	location / {
      		methods		GET POST;
      		access		true;
      		root		www/test/;
      		index		index.html;
      		autoindex	false;
      	}
      	location /assets/ {
      		methods		GET;
      		root		www/assets/;
      		autoindex	true;
      	}
      	location /fpm/ {
      		methods		GET POST;
      		access		true;
      		root		cgi/test-php/;
      		index		info.php;
              fastcgi_pass    var/php-fpm.d/tmp/php-fpm.sock;
      		autoindex	false;
      	}
      	location /testcgi/ {
      		methods		GET POST;
      		access		false;
      		root		www/;
      		index		index.html;
      		autoindex	false;
      	}
      	location /bonjour/ {
      		methods		GET POST;
      		access		true;
      		root		www/bonjour/;
      		index		index.html;
      		autoindex		false;
      	}
      	location /database/ {
      		methods		GET POST;
      		access		true;
      		root		www/database/;
      		clientMaxBodySize		45mo;
              # index       index.html;
      		autoindex		true;
      	}
      	location ~ \.sh$ {
      		cgi_bin			bash;
      	}
      	location ~ \.php$ {
      		cgi_bin			php-cgi82;
      	}
      	location ~ \.py$ {
      		cgi_bin			python3;
      	}
      	location /database/db/salut/bonjour/lol/42/ {
      		return 301 /database/;
      	}
      }
     ```

2. **Static File Serving**:
   - Requests for static files are resolved based on the `root` and `index` directives in the configuration.

3. **Dynamic Content (CGI)**:
   - For paths defined as CGI handlers, WebServ spawns the specified script, passing the request as environment variables.

4. **FastCGI**:
   - Dynamic requests are forwarded to a FastCGI server using a socket, enabling high-performance interaction with programs like PHP-FPM.

---
## **Key Learnings**
  - Low-level socket programming for HTTP server implementation.
  - Process creation and management for executing CGI scripts.
  - Integration of FastCGI for high-performance dynamic content delivery.
  - HTTP/1.1 compliance and request parsing.
  - Efficient handling of multiple client connections using multiplexing.

---

## **Recommended Resources**

### **HTTP and Web Servers**
- [RFC 2616: HTTP/1.1 Specification](https://www.ietf.org/rfc/rfc2616.txt)  
- [RFC 7540: HTTP/2 Specification](https://www.rfc-editor.org/rfc/rfc7540)  
- [MDN Web Docs: HTTP Overview](https://developer.mozilla.org/en-US/docs/Web/HTTP/Overview)  
- [The Architecture of Open Source Applications: NGINX](https://aosabook.org/en/nginx.html)  

### **CGI and FastCGI**
- [FastCGI Documentation](https://fastcgi-archives.github.io/)  
- [CGI Programming 101](http://www.cgi101.com/book/)  
- [FastCGI: A High-Performance Gateway Interface](https://www.fastcgi.com/)  

### **Networking and Sockets**
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)  
- [UNIX Network Programming by W. Richard Stevens](https://www.amazon.com/UNIX-Network-Programming-Sockets-Networking/dp/0131411551)  
- [Linux Socket Programming by Example](https://nostarch.com/socket.htm)  

### **C++ and System Programming**
- [C++ Reference (cppreference.com)](https://en.cppreference.com/)  
- [The C++ Programming Language by Bjarne Stroustrup](https://www.amazon.com/C-Programming-Language-4th/dp/0321563840)  
- [Advanced Programming in the UNIX Environment by W. Richard Stevens](https://www.amazon.com/Advanced-Programming-UNIX-Environment-3rd/dp/0321637739)  

### **Web Security**
- [OWASP: HTTP Security Headers](https://owasp.org/www-project-secure-headers/)  
- [Understanding TLS and SSL](https://www.cloudflare.com/learning/ssl/what-is-ssl/)  
- [TLS/SSL Best Practices](https://bettertls.com/)  

### **Debugging and Performance**
- [Valgrind: Memory Debugging Tool](https://valgrind.org/)  
- [GDB: The GNU Debugger](https://www.gnu.org/software/gdb/)  
- [Perf: Linux Performance Analysis Tool](https://perf.wiki.kernel.org/index.php/Main_Page)  

---
