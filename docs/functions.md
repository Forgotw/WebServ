# 1. Functions

## 1.1. TOC

- [1. Functions](#1-functions)
	- [1.1. TOC](#11-toc)
	- [1.2. socket()](#12-socket)
		- [1.2.1. socket() - Prototype](#121-socket---prototype)
		- [1.2.2. socket() - Explications](#122-socket---explications)
		- [1.2.3. socket() - Exemple](#123-socket---exemple)
	- [1.3. bind()](#13-bind)
		- [1.3.1. bind() - Prototype](#131-bind---prototype)
		- [1.3.2. bind() - Explications](#132-bind---explications)
			- [1.3.2.1. const struct sockaddr \*addr on INET](#1321-const-struct-sockaddr-addr-on-inet)
			- [1.3.2.2. const struct sockaddr \*addr on INET6](#1322-const-struct-sockaddr-addr-on-inet6)
		- [1.3.3. bind() - Exemple](#133-bind---exemple)
		- [1.3.4. bind() - Notes](#134-bind---notes)
	- [1.4. htonl()](#14-htonl)
		- [1.4.1. htonl() - Prototype](#141-htonl---prototype)
		- [1.4.2. htonl() - Explications](#142-htonl---explications)
		- [1.4.3. htonl() - Exemple](#143-htonl---exemple)
	- [1.5. htons()](#15-htons)
		- [1.5.1. htons() - Prototype](#151-htons---prototype)
		- [1.5.2. htons() - Explications](#152-htons---explications)
		- [1.5.3. htons() - Exemple](#153-htons---exemple)
	- [1.6. ntohl()](#16-ntohl)
		- [1.6.1. ntohl() - Prototype](#161-ntohl---prototype)
		- [1.6.2. nthohl() - Explications](#162-nthohl---explications)
		- [1.6.3. ntohl() - Exemple](#163-ntohl---exemple)
	- [1.7. ntohs()](#17-ntohs)
		- [1.7.1. ntohs() - Prototype](#171-ntohs---prototype)
		- [1.7.2. ntohs() - Explications](#172-ntohs---explications)
		- [1.7.3. ntohs() - Exemple](#173-ntohs---exemple)
	- [1.8. listen()](#18-listen)
		- [1.8.1. listen() - Prototype](#181-listen---prototype)
		- [1.8.2. listen() - Explications](#182-listen---explications)
		- [1.8.3. listen() - Exemple](#183-listen---exemple)
	- [1.9. accept()](#19-accept)
		- [1.9.1. accept() - Prototype](#191-accept---prototype)
		- [1.9.2. accept() - Explications](#192-accept---explications)
		- [1.9.3. accept() - Exemple](#193-accept---exemple)
	- [1.10. connect()](#110-connect)
		- [1.10.1. connect() - Prototype](#1101-connect---prototype)
		- [1.10.2. connect() - Explications](#1102-connect---explications)
		- [1.10.3. connect() - Exemple](#1103-connect---exemple)
	- [1.11. send()](#111-send)
		- [1.11.1. send() - Prototype](#1111-send---prototype)
		- [1.11.2. send() - Explications](#1112-send---explications)
		- [1.11.3. send() - Exemple](#1113-send---exemple)
	- [1.12. recv()](#112-recv)
		- [1.12.1. recv() - Prototype](#1121-recv---prototype)
		- [1.12.2. recv() - Explications](#1122-recv---explications)
		- [1.12.3. recv() - Exemple](#1123-recv---exemple)
	- [1.13. select()](#113-select)
		- [1.13.1. select() - Prototype](#1131-select---prototype)
		- [1.13.2. select() - Explications](#1132-select---explications)
		- [1.13.3. select() - Macros](#1133-select---macros)
			- [1.13.3.1. FD\_ZERO()](#11331-fd_zero)
			- [1.13.3.2. FD\_SET()](#11332-fd_set)
			- [1.13.3.3. FD\_CLR()](#11333-fd_clr)
			- [1.13.3.4. FD\_ISSET()](#11334-fd_isset)
		- [1.13.4. select() - Exemple](#1134-select---exemple)
	- [1.14. gai\_strerror](#114-gai_strerror)
	- [1.15. socketpair](#115-socketpair)
	- [1.16. getaddrinfo](#116-getaddrinfo)
	- [1.17. freeaddrinfo](#117-freeaddrinfo)
	- [1.18. setsockopt](#118-setsockopt)
	- [1.19. getsockname](#119-getsockname)
	- [1.20. getprotobyname](#120-getprotobyname)

## 1.2. socket()

### 1.2.1. socket() - Prototype

```c
#include <sys/type.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

### 1.2.2. socket() - Explications

La fonction **socket()** permet de créer un *point d'arrivée* pour la communication et retourne un descripteur de fichier qui référence ce point d'arrivée.

Son paramètre *domain* permet de spécifier la famille de protocole que nous voulons utiliser pour communiquer. Ici, deux domain nous intéresse :

- **AF_INET** : pour IPv4
- **AF_INET6** : pour IPv6

Le paramètre *type* permet de spécifié la sémantique de communication.

- **SOCK_STREAM** : pour TCP (que nous allons utiliser pour ce projet)
- **SOCK_DGRAM** : pour UDP

---

> /!\ La partie suivante n'est peu être pas compatible avec MacOS (de ces morts...)

Le paramètre *type* permet aussi de spécifié un type de socket. Pour se faire, nous devons passer notre choix de sémantique suivit suivit du bitwise operator *OR*, suivit de l'une des valeurs suivante :

- **SOCK_NONBLOCK** : Ajoute le flag **O_NONBLOCK** au descripteur de fichier retourner par la fonction *socket()*. Voir `man 2 open`. Cela permet de s'économisé un appel à *fcntl()* mais je ne sais pas si c'est autorisé dans le cadre du projet car la fonction *fcntl()* est réglementée.
- **SOCK_CLOEXEC** : Ajoute le flag **FD_CLOEXEC** (close-on-exec) au descripteur de fichier.

---

Si l'appel à *socket()* échoue, -1 est retourné et errno est set. Autrement, la fonction retourne un descripteur de fichier.

### 1.2.3. socket() - Exemple

```cpp
#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>
#include <cerrno>
#include <cstring>

int main () {
	int fd_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

	if (fd_socket == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return (1);
	}
	std::cout << "fd_socket : " << fd_socket << std::endl;
	return 0;
}
```

```text
fd_socket : 3
```

## 1.3. bind()

### 1.3.1. bind() - Prototype

```cpp
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

### 1.3.2. bind() - Explications

Une fois que nous créons un socket avec la fonction *socket()*, il existe dans le namespace mais n'a ni adresse ni port. La fonction **bind()** permet d'assigner une adresse via le paramètre *addr* au socket passé par le paramètre *sockfd*. *addrlen* spécifie la taille en bytes de la structure *addr*.

Il est obligatoire d'assigné une adresse en utilisant **bind()**  avec qu'un socket **SOCK_STREAM** puisse recevoir une connexion.

La structure *addr* dépend du domain utiliser par le socket :

#### 1.3.2.1. const struct sockaddr *addr on INET

```cpp
struct sockaddr_in {
	sa_family_t		sin_family;	/* address family: AF_INET */
	in_port_t		sin_port;	/* port in network byte order */
	struct in_addr	sin_addr;	/* internet address */
};

/* Internet address. */
struct in_addr {
	uint32_t	s_addr;	/* address in network byte order */
};
```

voir : `man 7 ip`

#### 1.3.2.2. const struct sockaddr *addr on INET6

```cpp
struct sockaddr_in6 {
	sa_family_t		sin6_family;	/* AF_INET6 */
	in_port_t		sin6_port;		/* port number */
	uint32_t		sin6_flowinfo;	/* IPv6 flow information */
	struct in6_addr	sin6_addr;		/* IPv6 address */
	uint32_t		sin6_scope_id;	/* Scope ID (new in 2.4) */
};

struct in6_addr {
	unsigned char	s6_addr[16];	/* IPv6 address */
};
```

voir : `man 7 ipv6`

### 1.3.3. bind() - Exemple

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <cerrno>
#include <cstring>

int main () {
	int fd_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

	if (fd_socket == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return (1);
	}
	std::cout << "fd_socket : " << fd_socket << std::endl;


	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));		//On set la structure à 0
	addr.sin_family = AF_INET;					//On set le protocole (IPv4)
	addr.sin_addr.s_addr = htonl(0x7F000001);	//On set l'adresse IP (127.0.0.1)
	addr.sin_port = htons(8080);				//On set le port
	if (bind(fd_socket, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return (2);
	}
	std::cout << "Socket lié avec le port 8080" << std::endl;

	return 0;
}
```

```text
fd_socket : 3
Socket lié avec le port 8080
```

### 1.3.4. bind() - Notes

- Nous devons passer l'adresse IP sous la forme d'un entier non signé de 32 bits. L'adresse est représenter sous forme hexadécimal ou chaque partie de l'adresse IP est encoder sur 2 caractère. C'est pour cela que dans l exemple, j'utilise comme adresse 0x7F000001, `7F` vaut `127`, `00` vaut `0` et `01` vaut `1`.
- J'utilise la fonction **htonl()** dans l'exemple. Je dois l'utiliser car *bind()* s'attend a un entier non signé sur 32 bits sous forme hexadécimal encoder en *big-endian*. Il a un chapitre dédié à cette fonction dans ce document.
- J'utilise aussi la fonction **htons()** pour set le port. Cette fonction aussi envoie le port encoder en *big-endian* à *bind()*. Il y a aussi un chapitre dédié à cette fonction dans ce document.

## 1.4. htonl()

### 1.4.1. htonl() - Prototype

```cpp
#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong);
```

### 1.4.2. htonl() - Explications

La fonction **htonl()** (host to network long) convertit l'entier non signé sur 32 bits *hostlong* avec les bytes ordonné selon l’hôte (*little-endian*) en entier non signé de 32 bits ou les bytes sont ordonné pour le réseau (*big-endian*).

Cette fonction est utilisée pour modifier une adresse IPv4 encodée en hexadécimal avec les bytes ordonné selon l'hôte.

### 1.4.3. htonl() - Exemple

```cpp
#include <arpa/inet.h>

#include <iostream>

int main () {
	uint32_t ip_addr = 0x7F000001;

	std::cout << "ip_addr : " << std::hex << ip_addr << std::endl;
	std::cout << "htonl(ip_addr) : " << std::hex << htonl(ip_addr) << std::endl;
	return 0;
}
```

```text
ip_addr : 7f000001
htonl(ip_addr) : 100007f
```

## 1.5. htons()

### 1.5.1. htons() - Prototype

```cpp
#include <arpa/inet.h>

uint16_t htons(uint16_t hostshort);
```

### 1.5.2. htons() - Explications

La fonction **htons()** (host to network short) convertit l'entier non signé sur 16 bits *hostshort* avec les bytes ordonée selon l'hôte (*little-endian*) en entier non signé de 16 bits ou les bytes sont ordonné pour le réseau (*big-endian*).

Cette fonction est utilisée pour changer le port avec les bytes ordonné pour l'hôte en port avec les bytes ordonné pour le réseau.

### 1.5.3. htons() - Exemple

```cpp
#include <arpa/inet.h>

#include <iostream>

int main () {
	uint16_t port = 4242;

	std::cout << "port : " << port << std::endl;
	std::cout << "port hex : " << std::hex << port << std::endl;
	std::cout << "htons(port) : " << std::hex << htons(port) << std::endl;
	return 0;
}
```

```text
port : 4242
port hex : 1092
htons(port) : 9210
```

## 1.6. ntohl()

### 1.6.1. ntohl() - Prototype

```cpp
#include <arpa/inet.h>

uint32_t ntohl(uint32_t netlong);
```

### 1.6.2. nthohl() - Explications

La fonction **ntohl()** (network to host long) fait l'inverse de la fonction **htonl()**. Elle prend un *uint32_t* ordonné selon le réseau (*big-endian*) et le retourne ordonné selon l'hôte (*little-endian*).

### 1.6.3. ntohl() - Exemple

```cpp
#include <arpa/inet.h>

#include <iostream>

int main () {
	uint32_t ip_addr_base = 0x7F000001;
	uint32_t ip_addr_convert = htonl(ip_addr_base);
	uint32_t result = ntohl(ip_addr_convert);

	std::cout << "ip_addr_base : " << std::hex << ip_addr_base << std::endl;
	std::cout << "ip_addr_convert : " << std::hex << ip_addr_convert << std::endl;
	std::cout << "result : " << std::hex << result << std::endl;
	std::cout << "result == ip_addr_base : " << std::boolalpha << (result == ip_addr_base ? true : false) << std::endl;
	return 0;
}
```

```text
ip_addr_base : 7f000001
ip_addr_convert : 100007f
result : 7f000001
result == ip_addr_base : true
```

## 1.7. ntohs()

### 1.7.1. ntohs() - Prototype

```cpp
#include <arpa/inet.h>

uint16_t ntohs(uint16_t netshort);
```

### 1.7.2. ntohs() - Explications

La fonction **ntohs()** (network to host short) fait l'inverse de la fonction **htons()**. Elle prend un *uint16_t* ordonné selon le réseau (*big-endian*) et le retourne ordonné selon l'hôte (*little-endian*).

### 1.7.3. ntohs() - Exemple

```cpp
#include <arpa/inet.h>

#include <iostream>

int main () {
	uint16_t port_base = 4242;
	uint16_t port_convert = htons(port_base);
	uint16_t result = ntohs(port_convert);

	std::cout << "port_base : " << port_base << std::endl;
	std::cout << "port_base hex : " << std::hex << port_base << std::endl;
	std::cout << "port_convert : " << std::hex << port_convert << std::endl;
	std::cout << "result : " << result << std::endl;
	std::cout << "result == port_base : " << std::boolalpha << (result == port_base ? true : false) << std::endl;
	return 0;
}
```

```text
port_base : 4242
port_base hex : 1092
port_convert : 9210
result : 1092
result == port_base : true
```

## 1.8. listen()

### 1.8.1. listen() - Prototype

```cpp
#include <sys/types.h>
#include <sys/socket.h>

int listen(int sockfd, int backlog);
```

### 1.8.2. listen() - Explications

La fonction **listen()** marque un socket référencé *sockfd* comme étant un socket passif, c'est à dire, un socket qui sera utiliser pour reçevoir des connections.

Le paramètre *sockfd* est un descripteur de fichier sur un socket.

Le parmètre *backlog* représente la taille de la queue de connexion en attente. Si la taille de la queue est plus dépasse se nombre, les nouvelle connexion ne seront pas accéptée.

Si la fonction se déroule correctement, *0* est retourné alors que si la fonction échoue *-1* est retourné et *errno* et set.

### 1.8.3. listen() - Exemple

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>

#include <iostream>
#include <cerrno>
#include <cstring>

#define LISTEN_BACKLOG 42

int main () {
	int fd_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

	if (fd_socket == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return (1);
	}
	std::cout << "fd_socket : " << fd_socket << std::endl;

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(0x7F000001);
	addr.sin_port = htons(8080);
	if (bind(fd_socket, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return (2);
	}
	std::cout << "Socket lié avec le port 8080" << std::endl;

	if(listen(fd_socket, LISTEN_BACKLOG) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return (3);
	}
	std::cout << "Server is running on port 8080" << std::endl;

	return 0;
}
```

```text
fd_socket : 3
Socket lié avec le port 8080
Server is running on port 8080
```

## 1.9. accept()

### 1.9.1. accept() - Prototype

```cpp
#include <sys/types.h>
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

### 1.9.2. accept() - Explications

La fonction **accept()** permet d'extraire la première connexion de la queue des connexion en attente d'un socket qui en est mode listen.

Le paramère *sockfd* et le descripteur de fichier du socket qui a été *bind()* et *listen()*.

Le paramètre *addr* est une structure qui va reçevoir les informations de la connexion du client.

Le paramètre *addrlen* va reçwvoir la taille de la structure *addr* retournée.

Si **accept()** reçoit une nouvelle connexion, un descripteur de fichier est retourné, sinon, *-1*.

### 1.9.3. accept() - Exemple

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <cerrno>
#include <cstring>

#define LISTEN_BACKLOG 42

int main () {
	int fd_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (fd_socket == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 1;
	}

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(0x7F000001);
	addr.sin_port = htons(8080);
	if (bind(fd_socket, (struct sockaddr*) &addr, sizeof(addr)) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 2;
	}

	if(listen(fd_socket, LISTEN_BACKLOG) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 3;
	}
	std::cout << "Server is running on port 8080" << std::endl;

	struct sockaddr_in peer_addr;
	socklen_t peer_len;
	int fd_peer;
	std::cout << "waiting ..." << std::endl;
	for (;;)
	{
		peer_len = sizeof(peer_addr);
		fd_peer = accept(fd_socket, (struct sockaddr *) &peer_addr, &peer_len);
		if (fd_peer != -1)
			break;
	}
	std::cout << "New connection !!!" << std::endl;
	close(fd_peer);
	close(fd_socket);
	return 0;
}
```

```bash
# Server
Server is running on port 8080
waiting ...
```

```bash
# Client
nc 127.0.0.1 8080
```

```bash
# Server
Server is running on port 8080
waiting ...
New connection !!!
```

## 1.10. connect()

### 1.10.1. connect() - Prototype

```cpp
#include <sys/types.h>
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

### 1.10.2. connect() - Explications

La fonction **connect()** connecte le socket (*sockfd*) passé en paramètre à l'adresse spécifiée par le paramètre *addr*. Le paramètre *addrlen* corréspond à la taille de *addr*.

Si une erreur survient durant l'exécution de **connect()**, -1 est retourné et errno est set.

### 1.10.3. connect() - Exemple

```cpp
// Server
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <cerrno>
#include <cstring>

#define LISTEN_BACKLOG 42

int main () {
	int fd_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_socket == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 1;
	}

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(0x7F000001);
	addr.sin_port = htons(8080);
	if (bind(fd_socket, (struct sockaddr*) &addr, sizeof(addr)) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 2;
	}

	if(listen(fd_socket, LISTEN_BACKLOG) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 3;
	}
	std::cout << "Server is running on port 8080" << std::endl;

	struct sockaddr_in peer_addr;
	socklen_t peer_len;
	int fd_peer;
	std::cout << "waiting ..." << std::endl;
	for (;;)
	{
		peer_len = sizeof(peer_addr);
		fd_peer = accept(fd_socket, (struct sockaddr *) &peer_addr, &peer_len);
		if (fd_peer != -1)
			break;
	}
	std::cout << "New connection !!!" << std::endl;
	close(fd_peer);
	close(fd_socket);
	return 0;
}
```

```cpp
// Client
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <cerrno>
#include <cstring>

int main () {
	int fd_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (fd_socket == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 1;
	}

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(0x7F000001);
	addr.sin_port = htons(8080);

	if (connect(fd_socket, (struct sockaddr *) &addr, sizeof(addr)) > 0) {
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 2;
	}
	std::cout << "Connected !!" << std::endl;
	close(fd_socket);
	return 0;
}
```

```bash
# Server
Server is running on port 8080
waiting ...
```

```bash
# Client
Connected !!
```

```bash
# Server
Server is running on port 8080
waiting ...
New connection !!!
```

## 1.11. send()

### 1.11.1. send() - Prototype

```cpp
#include <sys/types.h>
#include <sys/socket.h>

ssize_t send(int sockfd, const void *buf, size_t len, int flag);
```

### 1.11.2. send() - Explications

La fonction **send()** permet de transmettre à message à un socket. Cette fonction doit être utilisée seulement quand le socket à le status *connected*. La seul différence entre **send()** et **write()** et la présence du paramètre *flag*. utiliser **send()** avec un flag de *0* est similaire à utiliser **write()**.

Le paramètre *sockfd* est le socket par le quel nous voulons envoyer notre message.

Le paramètre *buf* contient le message que nous voulons envoyer et le paramètre *len* est la longeur du message.

Pour la liste des flags, voir `man send`.

Si **send()** se déroule corréctement, elle renvoie le nombre de bytes qui a été envoyé. Sinon, -1 est retourné et errno est set.

### 1.11.3. send() - Exemple

```cpp
// Server
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <cerrno>
#include <cstring>

#define LISTEN_BACKLOG 42

int main () {
	int fd_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_socket == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 1;
	}

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(0x7F000001);
	addr.sin_port = htons(8080);
	if (bind(fd_socket, (struct sockaddr*) &addr, sizeof(addr)) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 2;
	}

	if(listen(fd_socket, LISTEN_BACKLOG) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 3;
	}
	std::cout << "Server is running on port 8080" << std::endl;

	struct sockaddr_in peer_addr;
	socklen_t peer_len;
	int fd_peer;
	std::cout << "waiting ..." << std::endl;
	for (;;)
	{
		peer_len = sizeof(peer_addr);
		fd_peer = accept(fd_socket, (struct sockaddr *) &peer_addr, &peer_len);
		if (fd_peer != -1)
			break;
	}
	std::cout << "New connection !" << std::endl;
	char msg[] = "Welcome !";
	if (send(fd_peer, (const void *) &msg, std::strlen(msg), 0) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 4;
	}
	std::cout << "The message was send !" << std::endl;
	close(fd_peer);
	close(fd_socket);
	return 0;
}
```

```cpp
// Client
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <cerrno>
#include <cstring>

int main () {
	int fd_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (fd_socket == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 1;
	}

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(0x7F000001);
	addr.sin_port = htons(8080);

	if (connect(fd_socket, (struct sockaddr *) &addr, sizeof(addr)) > 0) {
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 2;
	}
	std::cout << "Connected !" << std::endl;
	close(fd_socket);
	return 0;
}
```

```bash
# Server
Server is running on port 8080
waiting ...
```

```bash
# Client
Connected !
```

```bash
Server is running on port 8080
waiting ...
New connection !
The message was send !
```

## 1.12. recv()

### 1.12.1. recv() - Prototype

```cpp
#include <sys/types.h>
#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t len, int flags)
```

### 1.12.2. recv() - Explications

La fonction **recv()** permet de reçevoir un message depuis un socket. Cette fonction doit être utilisée seulement quand le socket à le status *connected*. La seul différence entre **recv()** et **read()** et la présence du paramètre *flags*. Utiliser **recv()** avec un flag de *0* est similaire à utiliser **read()**.

Le paramètre *sockfd* est le socket par le quel nous allons reçevoir le message.

Le paramètre *buf* est le buffer dans le quel sera écrit le message reçu et le paramètre *len* est la taille du buffer.

Pour la liste des flags, voir `man send`.

Si **recv()** se déroule corréctement, elle renvoie le nombre de bytes qui a été reçu. Sinon, -1 est retourné et errno est set.

### 1.12.3. recv() - Exemple

```cpp
// Server
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <cerrno>
#include <cstring>

#define LISTEN_BACKLOG 42

int main () {
	int fd_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_socket == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 1;
	}

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(0x7F000001);
	addr.sin_port = htons(8080);
	if (bind(fd_socket, (struct sockaddr*) &addr, sizeof(addr)) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 2;
	}

	if(listen(fd_socket, LISTEN_BACKLOG) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 3;
	}
	std::cout << "Server is running on port 8080" << std::endl;

	struct sockaddr_in peer_addr;
	socklen_t peer_len;
	int fd_peer;
	std::cout << "waiting ..." << std::endl;
	for (;;)
	{
		peer_len = sizeof(peer_addr);
		fd_peer = accept(fd_socket, (struct sockaddr *) &peer_addr, &peer_len);
		if (fd_peer != -1)
			break;
	}
	std::cout << "New connection !" << std::endl;
	char msg[] = "Welcome !";
	if (send(fd_peer, (const void *) &msg, std::strlen(msg), 0) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 4;
	}
	std::cout << "The message was send !" << std::endl;
	close(fd_peer);
	close(fd_socket);
	return 0;
}
```

```cpp
// Client
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <cerrno>
#include <cstring>

int main () {
	int fd_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_socket == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 1;
	}

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(0x7F000001);
	addr.sin_port = htons(8080);

	if (connect(fd_socket, (struct sockaddr *) &addr, sizeof(addr)) > 0) {
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 2;
	}
	std::cout << "Connected !" << std::endl;
	char buffer[1024];
	ssize_t rec_size = 0;
	rec_size = recv(fd_socket, (void *) &buffer, 1024, 0);
	if (rec_size == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 3;
	}
	if (rec_size == 0)
		return 4;

	std::cout << "Nouveau message" << std::endl;
	buffer[rec_size] = '\0';
	std::cout << "-> " << buffer << std::endl;
	close(fd_socket);
	return 0;
}
```

```bash
# Server
Server is running on port 8080
waiting ...
```

```bash
# Client
Connected !
Nouveau message
-> Welcome !
```

```bash
# Server
Server is running on port 8080
waiting ...
New connection !
The message was send !
```

## 1.13. select()

### 1.13.1. select() - Prototype

```cpp
#include <sys/select>

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, timeval *timeout);

void FD_CLR(int fd, fd_set *set);
int FD_ISSET(int fd, fd_set *set);
void FD_SET(int fd, fd_set *set);
void FD_ZERO(fd_set *set);
```

### 1.13.2. select() - Explications

La fonction **select()** permet à un programme de surveillier plusieurs file descriptor, en attendant qu'un ou plusieurs d'entre eux soient "prêt" pour une certaine catégorie d'opération d'I/O. Un descripteur de fichier est considéré comme prêt s'il est possible d'effectuer une opération d'I/O correspondante (par exemple, read()/recv() et write()/send()) sans blocage.

La fonction **select()** ne peut surveillier seulement un nombre de descripteur de fichier plus petit que **FD_SIZE**.

Les paramètre principaux de la fonction **select()** sont les trois *sets* de descripteur de fichier. Chaqun de ces sets (de type *fd_set*) représtent l'evenement que l'appelant attend de pouvoir traiter. Chaqun de ces trois paramètre peu être set à NULL si nous ne voulons pas traiter ce type d'événement.

*readfds* est le *set* des descripteurs qu'il faut suivre pour voir si ils sont disponbile pour la lecteur. Un descripteur de fichier qui est condiédrer comme prêt pour la lecture est un descripteur ou l'opération de lecture ne bloquerait pas. Un descripteur et aussi prêt à la lecture quand il est au EOF. Une fois que **select()** à retourné, tous les descipteur dans *readfds* sont supprimer, à l'exception des descripteur qui sont prêt pour la lecture.

*writefds* est le *set* des descripteurs qu'il faut suivre pour voir si ils sont disponible pour l'écriture. Un descripteur de fichier qui est considéré comme prêt pour l'écriture est un descripteur ou l'opération d'écriture ne bloquerait pas. Pourtant, même si un descripteur inique qu'il est inscriptible, l'écriture d'une gross masse d'informations pourrait quand même bloquer. Une fois que **select()** à retourné, tous les descripteur dans *wrirtefds* sont supprimé, à l'excecption des descripteurs qui sont prêt pour l'écriture.

*exceptfds* est le *set* des descripteurs qu'il faut suivre pour voir pour des *condition exceptionel*. Voir `man 2 poll` section `POLLPRI`.

Le paramètre *nfds* détérmine le numéro du descipteur de fichier le plus éléveé plus 1. Ce chiffre détérmine jusqu'a quel descripteur **select()** va itérer sur les trois *set*.

Le paramètre *timeout* est une structure *timeval* qui définit l'intérval durant le quel est **select()** doit bloquer pour attendre qu'un descripteur de fichier deviennt prêt. L'appel va bloquer jusqu'a :

- un descripteur devient prêt
- l'appel est intérompu un handler de signal
- *timeout* expire

La structure *timeval* est définie comme suite:

```cpp
struct timeval {
	time_t		tv_sec;		/* seconds */
	suseconds_t	tv_usec;	/* microseconds */
}
```

Si tous les paramètre de **select()** sont 0, la fonction retourne directement.

Si le paramètre *timeout* est à 0, **select()** bloque indéfiniment jusqu'a qu'un descripteur deviennt prêt.

Si la foncontion **select()** se déroule sans erreur, elle retourne le nombre de descripteur de fichier contenu dans les trois *sets*. Elle retourne 0 si le *timeout* a été atteind et en cas d'erreur, elle retourne -1 et errno est set.

Pour manipuler les *fd_set*, nous avons à notre disposition des macros.

### 1.13.3. select() - Macros

#### 1.13.3.1. FD_ZERO()

Cette macro retirer tous les descripteur de fichier du set paser en paramètre. Elle doit être utilisée comme première étape afin d'initialiser un set.

#### 1.13.3.2. FD_SET()

Cette macro ajoute le descripteur de fichier *fd* au *set*. Ajoute un descripteur qui est déjà présent dans le set ne fait rien et ne produit pas d'erreur.

#### 1.13.3.3. FD_CLR()

Cette fonction retire le descripteur de fichier *fd* du *set*. Supprimer un descripteur qui n'est pas présent dans le set ne fait rien et ne génére pas d'erreur.

#### 1.13.3.4. FD_ISSET()

La fonction **select()** modifie le contenu des set en fonction des règle énoncée plus haut. Après un appel à **select()**, la macro **FD_ISSET()** peu être utilisée afin de détérminer si un descripteur de fichier est encore présent dans le set. **FD_ISSET()** retourne pas 0 si un descripteur *fd* est dans le *set* et 0 si il n'y est pas.

### 1.13.4. select() - Exemple

```cpp
// Server
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <cerrno>
#include <cstring>

#define LISTEN_BACKLOG 42

int main () {
	int fd_socket1 = socket(AF_INET, SOCK_STREAM, 0);
	int fd_socket2 = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_socket1 == -1 || fd_socket2 == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 1;
	}

	struct sockaddr_in addr1;
	std::memset(&addr1, 0, sizeof(addr1));
	addr1.sin_family = AF_INET;
	addr1.sin_addr.s_addr = htonl(0x7F000001);
	addr1.sin_port = htons(8080);
	if (bind(fd_socket1, (struct sockaddr *) &addr1, sizeof(addr1)) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 2;
	}
	struct sockaddr_in addr2;
	std::memset(&addr2, 0, sizeof(addr2));
	addr2.sin_family = AF_INET;
	addr2.sin_addr.s_addr = htonl(0x7F000001);
	addr2.sin_port = htons(4242);
	if(bind(fd_socket2, (struct sockaddr *) &addr2, sizeof(addr2)) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 3;
	}

	if (listen(fd_socket1, LISTEN_BACKLOG) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 4;
	}
	std::cout << "Server is running on port 8080" << std::endl;
	if (listen(fd_socket2, LISTEN_BACKLOG) == -1)
	{
		std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
		return 5;
	}
	std::cout << "Server is running on port 4242" << std::endl;

	fd_set read_fds;
	int fd_max = fd_socket2;

	for (;;)
	{
		FD_ZERO(&read_fds);
		FD_SET(fd_socket1, &read_fds);
		FD_SET(fd_socket2, &read_fds);

		if (select(fd_max + 1, &read_fds, NULL, NULL, NULL) == -1)
		{
			std::cerr  << "[!] -> " << std::strerror(errno) << std::endl;
			return 6;
		}
		struct sockaddr_in peer_addr;
		socklen_t peer_len;
		int fd_peer;
		for (int i = 0; i <= fd_max; i++)
		{
			if (FD_ISSET(i, &read_fds))
			{
				if (i == fd_socket1)
				{
					std::cout << "New connection on port 8080" << std::endl;
					peer_len = sizeof(peer_addr);
					fd_peer = accept(fd_socket1, (struct sockaddr *) &peer_addr, &peer_len);
				}
				else if (i == fd_socket2)
				{
					std::cout << "New connection on port 4242" << std::endl;
					peer_len = sizeof(peer_addr);
					fd_peer = accept(fd_socket2, (struct sockaddr *) &peer_addr, &peer_len);
				}
				if (fd_peer == -1)
				{
					std::cerr << "[!] -> " << std::strerror(errno) << std::endl;
					return 7;
				}
				close(fd_peer);
			}
		}
	}
	close(fd_socket1);
	close(fd_socket2);
	return 0;
}
```

Ce serveur attend indéfiniment des connexion sur deux port différent et utilise **select()** pour savoir quand une connexion à été éffectuer. Le serveur ne fait rien du client, il ferme la connexion directement. C'est juste un code d'exemple pour illustrer l'utilisation de **select()** dans le cas de l'écoute sur plusieurs socket.

## 1.14. gai_strerror

## 1.15. socketpair

## 1.16. getaddrinfo

## 1.17. freeaddrinfo

## 1.18. setsockopt

## 1.19. getsockname

## 1.20. getprotobyname
