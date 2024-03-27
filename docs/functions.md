# 1. Function

## 1.1. TOC

- [1. Function](#1-function)
	- [1.1. TOC](#11-toc)
	- [1.2. socket()](#12-socket)
		- [1.2.1. socket() - Prototype](#121-socket---prototype)
		- [1.2.2. socket() - Explications](#122-socket---explications)
		- [1.2.3. socket() - Exemple](#123-socket---exemple)
	- [1.3. gai\_strerror](#13-gai_strerror)
	- [1.4. socketpair](#14-socketpair)
	- [1.5. htons](#15-htons)
	- [1.6. htonl](#16-htonl)
	- [1.7. ntohs](#17-ntohs)
	- [1.8. ntohl](#18-ntohl)
	- [1.9. select](#19-select)
	- [1.10. accept](#110-accept)
	- [1.11. listen](#111-listen)
	- [1.12. send](#112-send)
	- [1.13. recv](#113-recv)
	- [1.14. chdir](#114-chdir)
	- [1.15. bind](#115-bind)
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
#include <errno.h>
#include <string.h>
#include <stdio.h>

int main () {
	int fd_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

	if (fd_socket == -1)
	{
		printf("[!] : %s\n", strerror(errno));
		return (1);
	}
	else
	{
		printf("fd_socket : %d\n", fd_socket);
		return 0;
	}
}
```

```text
> fd_socket : 3
```

## 1.3. gai_strerror

## 1.4. socketpair

## 1.5. htons

## 1.6. htonl

## 1.7. ntohs

## 1.8. ntohl

## 1.9. select

## 1.10. accept

## 1.11. listen

## 1.12. send

## 1.13. recv

## 1.14. chdir

## 1.15. bind

## 1.16. connect

## 1.17. getaddrinfo

## 1.18. freeaddrinfo

## 1.19. setsockopt

## 1.20. getsockname

## 1.21. getprotobyname
