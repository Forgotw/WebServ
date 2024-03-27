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
		- [bind() - Notes](#bind---notes)
	- [1.4. gai\_strerror](#14-gai_strerror)
	- [1.5. socketpair](#15-socketpair)
	- [1.6. htons](#16-htons)
	- [1.7. htonl](#17-htonl)
	- [1.8. ntohs](#18-ntohs)
	- [1.9. ntohl](#19-ntohl)
	- [1.10. select](#110-select)
	- [1.11. accept](#111-accept)
	- [1.12. listen](#112-listen)
	- [1.13. send](#113-send)
	- [1.14. recv](#114-recv)
	- [1.15. chdir](#115-chdir)
	- [1.16. connect](#116-connect)
	- [1.17. getaddrinfo](#117-getaddrinfo)
	- [1.18. freeaddrinfo](#118-freeaddrinfo)
	- [1.19. setsockopt](#119-setsockopt)
	- [1.20. getsockname](#120-getsockname)
	- [1.21. getprotobyname](#121-getprotobyname)

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

### bind() - Notes

- Nous devons passer l'adresse IP sous la forme d'un entier de 32 bits. L'adresse est représenter sous forme hexadécimal ou chaque partie de l'adresse IP est encoder sur 2 caractère. C'est pour cela que dans l exemple, j'utilise comme adresse 0x7F000001, `7F` vaut `127`, `00` vaut `0` et `01` vaut `1`.
- J'utilise la fonction **htonl()** dans l'exemple. Je dois l'utiliser car *bind()* s'attend a un entier sur 32 bits sous forme hexadécimal encoder en *big-endian*. Il a un chapitre dédié à cette fonction dans ce document.
- J'utilise aussi la fonction **htons()** pour set le port. Cette fonction aussi envoie le port encoder en *big-endian* à *bind()*. Il y a aussi un chapitre dédié à cette fonction dans ce document.

## 1.4. gai_strerror

## 1.5. socketpair

## 1.6. htons

## 1.7. htonl

## 1.8. ntohs

## 1.9. ntohl

## 1.10. select

## 1.11. accept

## 1.12. listen

## 1.13. send

## 1.14. recv

## 1.15. chdir

## 1.16. connect

## 1.17. getaddrinfo

## 1.18. freeaddrinfo

## 1.19. setsockopt

## 1.20. getsockname

## 1.21. getprotobyname
