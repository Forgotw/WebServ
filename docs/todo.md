# TODO

## Connection Handler

- [ ] Le serveur ne doit pas être bloquant
- [ ] Une requête ne dois jamais rester en suspend indéfiniment
- [ ] Ne jamais vérifié la valeur de `errno` après un read ou un write
- [ ] Le serveur doit retourner les bons messages d'erreur si il ne peu pas traiter la requête
- [ ] Le serveur peu écouter sur plusieurs port en même temps

### select()

- [ ] Le serveur doit n'utiliser qu'un seul `select()` pour toutes les opérations d'entrée / sortie
- [ ] `select()` doit vérifié l'écriture et la lecture en même temps
- [ ] Toutes les opérations d'entrée / sortie doivent passé par `select()`
- [ ] Ne pas utiliser `select()` avant de lire la configuration
- [ ] Utiliser les macros `FD_SET`, `FD_CLR`, `FD_ISSET` et `FD_ZERO`

## Serveur

- [ ] Les code de réponse sont correcte
  - [ ] 200
  - [ ] 300
  - [ ] 400
  - [ ] 500
- [ ] Le serveur est des pages d'erreur par défaut
- [ ] Le serveur doit pouvoir servit un site statique complet
- [ ] Les clients peuvent uploader des fichiers
- [ ] Les bonnes méthodes sont implémentée
  - [ ] GET
  - [ ] POST
  - [ ] DELETE

## Configuration

- [ ] Permet de spécifié le port et l'host pour plusieurs serveur virtuel
- [ ] Permet de spécifié ou non un `server_name`
- [ ] Le premier server pour un host:port est le serveur par défaut qui dois répondre à toute les requêtes qui ne matche avec aucun autre serveur.
- [ ] Permet de configurer les pages d'erreur par défaut
- [ ] Permet de limiter la taille du body du client
- [ ] Permet de spécifié ds routes qui peuvent être composée des règles suivante
  - [ ] Définir une liste de méthode HTTP acceptée par la route
  - [ ] Définir une redirection HTTP
  - [ ] Définir un fichier ou un répertoire à partir duquel le fichier doit être recherché
  - [ ] Activé ou non le listing des répertoires
  - [ ] Définir un fichier par défaut comme réponse si la route est un répertoire
  - [ ] Exécuter les CGI en fonction de l extension des fichiers
  - [ ] Définir si la route est capable de recevoir les fichiers upload par le client et ou ils doivent être stocké
  - [ ]

## CGI
