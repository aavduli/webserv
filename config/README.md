# todo a faire pour jimmy
	file config par defaut
	console::log afficher configuration
	multiple port usage
	si location no root ? obligatoir ? comment jave fait deja ? 

# Config Parser - Guide pour Angela

API simple pour récupérer la configuration du serveur webserv.

## Usage basique

```cpp
#include "config/WebservConfig.hpp"

WebservConfig config("config/def.conf");

// Vérifier que la config est valide
if (!config.isValid()) {
    std::cerr << "Config error: " << config.getLastError() << std::endl;
    return 1;
}
```

## Directives serveur - Les plus importantes pour toi

```cpp
// Port et host du serveur
int port = config.getPort();						// 8080 (défaut: 80)
std::string host = config.getHost();				// "127.0.0.1" (défaut: "127.0.0.1")

// Limite taille requête (IMPORTANT pour MessageParser)
size_t maxContent = config.getMaxContentLength(); // 1048576 bytes (défaut: 1MB)

// Méthodes HTTP autorisées
std::vector<std::string> methods = config.getAllowedMethods(); // ["GET", "POST"] (défaut: ["GET"])

// Pages d'erreur personnalisées
std::string page404 = config.getErrorPage(404);  // "./www/errors/404.html" (ou "" si pas défini)
std::string page403 = config.getErrorPage(403);  // "" si pas de page custom
std::string page500 = config.getErrorPage(500);
```

## Validation requêtes HTTP

```cpp
// 1. Vérifier taille du body
size_t maxContent = config.getMaxContentLength();
if (requestContentLength > maxContent) {
    // Retourner 413 Request Entity Too Large
}

// 2. Vérifier méthode autorisée
std::vector<std::string> allowedMethods = config.getAllowedMethods();
bool isAllowed = false;
for (size_t i = 0; i < allowedMethods.size(); i++) {
    if (requestMethod == allowedMethods[i]) {
        isAllowed = true;
        break;
    }
}
if (!isAllowed) {
    // Retourner 405 Method Not Allowed
}

// 3. Valider header Host
std::string hostHeader = request.getHeader("Host");
if (!config.isValidHostHeader(hostHeader)) {
    // Retourner 400 Bad Request
}
```

## Locations - Routing avancé

```cpp
// Vérifier si une route spécifique existe
if (config.hasLocation("/api")) {
    // Utiliser config spécifique pour /api
    std::map<std::string, std::string> apiConfig = config.getLocationConfig("/api");

    // Récupérer directives de la location
    std::string methods = apiConfig["methods"];        // "GET POST" (vide = hérite serveur)
    std::string root = apiConfig["root"];              // "/var/www/api" (vide = hérite serveur)
    std::string redirect = apiConfig["return"];        // "301 https://..." (vide = pas de redirect)

} else {
    // Utiliser config serveur par défaut
}
```

## Héritage location → serveur

Si une directive est vide dans une location, elle hérite du serveur :

```cpp
std::map<std::string, std::string> location = config.getLocationConfig("/api");

// Si methods vide dans location → utilise serveur
if (location["methods"].empty()) {
    std::vector<std::string> methods = config.getAllowedMethods(); // Récupère du serveur
} else {
    // Parser les méthodes de la location
    ParsingUtils utils;
    std::vector<std::string> methods = utils.split(location["methods"], ' ');
}
```

**Défauts finaux si serveur aussi vide :**
- `methods` → `["GET"]`
- `root` → `"."` (dossier courant)
- `client_max_body_size` → `1048576` bytes (1MB)

## Gestion pages d'erreur

```cpp
std::string page404 = config.getErrorPage(404);

if (page404.empty()) {
    // Pas de page custom → générer HTML basique
    response = "<!DOCTYPE html><html><body><h1>404 Not Found</h1></body></html>";
} else {
    // Lire fichier custom depuis page404
    // (vérifier que le fichier existe)
}
```

---

<details>
<summary>🔧 Documentation technique (développement)</summary>

## Architecture du parser

```
config/
├── WebservConfig.hpp/cpp        ← API PRINCIPALE (pour Angela)
├── ConfigParser.hpp/cpp         ← Parse fichier → structs
├── ConfigValidator.hpp/cpp      ← Valide données
├── ParsingUtils.hpp/cpp         ← Utilitaires (split, trim, etc.)
├── FileReader.hpp/cpp           ← Lecture fichiers
└── ConfigData.hpp               ← Structures données
```

## API complète WebservConfig

### Getters serveur
```cpp
int getPort() const;                              // Parse "listen 8080"
std::string getHost() const;                      // Parse "host 127.0.0.1"
std::string getServerName() const;               // Parse "server_name localhost"
std::string getRoot() const;                     // Parse "root /var/www"
std::string getIndex() const;                    // Parse "index index.html"
std::vector<std::string> getAllowedMethods() const; // Parse "allow_methods GET POST"
std::string getErrorPage(int code) const;        // Parse "error_page 404 /404.html"
size_t getMaxContentLength() const;              // Parse "client_max_body_size 1M"
```

### Getters locations
```cpp
bool hasLocation(const std::string& path) const;
std::map<std::string, std::string> getLocationConfig(const std::string& path) const;
const std::map<std::string, std::map<std::string, std::string>>& getAllLocations() const;
```

### Validation
```cpp
bool isValid() const;                            // Config parsée avec succès ?
std::string getLastError() const;               // Message d'erreur détaillé
bool matchesServerName(const std::string& host) const;  // Valide Host header
bool isValidHostHeader(const std::string& host) const;  // Format Host valide
```

## Format fichier config supporté

```nginx
server {
    listen 8080;                             # Port (obligatoire)
    host 127.0.0.1;                         # IP (défaut: 127.0.0.1)
    server_name localhost;                   # Nom serveur
    root /var/www/html;                      # Dossier racine
    index index.html;                        # Page par défaut
    allow_methods GET POST DELETE;           # Méthodes HTTP
    client_max_body_size 1M;                 # Taille max body
    error_page 404 /error404.html;           # Pages d'erreur
    error_page 500 /error500.html;

    location /api {
        methods GET POST;                    # Méthodes pour cette route
        root /var/www/api;                   # Root spécifique
        autoindex off;                       # Listing dossier
    }

    location /redirect {
        return 301 https://google.com;       # Redirection
    }
}
```

## ParsingUtils - Fonctions utilitaires

| Fonction | Usage | Input | Output |
|----------|-------|-------|--------|
| `split()` | Découper string | `("GET POST", ' ')` | `["GET", "POST"]` |
| `trim()` | Nettoyer espaces | `"  value  "` | `"value"` |
| `parseSize()` | Convertir taille | `"1M"` | `1048576` |
| `isValidIP()` | Valider IP | `"127.0.0.1"` | `true` |
| `isValidPort()` | Valider port | `"8080"` | `true` |
| `isValidMethod()` | Valider méthode | `"GET"` | `true` |

```cpp
ParsingUtils utils;
std::vector<std::string> methods = utils.split("GET POST DELETE", ' ');
size_t bytes = utils.parseSize("10M");  // 10485760
bool valid = utils.isValidIP("127.0.0.1");
```

## Validation automatique

✅ **IP valides** : Format IPv4 seulement
✅ **Ports valides** : 1-65535
✅ **Méthodes HTTP** : GET, POST, DELETE uniquement
✅ **Tailles** : Conversion K/M/G → octets automatique
✅ **Redirections** : Codes 301, 302 uniquement
✅ **Syntaxe** : Accolades et directives vérifiées

## Exemple complet

```cpp
#include "config/WebservConfig.hpp"

int main() {
    WebservConfig config("config/mega.conf");

    if (!config.isValid()) {
        std::cerr << config.getLastError() << std::endl;
        return 1;
    }

    // Infos serveur
    std::cout << "Server: " << config.getServerName() << std::endl;
    std::cout << "Port: " << config.getPort() << std::endl;
    std::cout << "Host: " << config.getHost() << std::endl;

    // Méthodes autorisées
    std::vector<std::string> methods = config.getAllowedMethods();
    std::cout << "Methods: ";
    for (size_t i = 0; i < methods.size(); i++) {
        std::cout << methods[i] << " ";
    }
    std::cout << std::endl;

    // Locations
    const std::map<std::string, std::map<std::string, std::string>>& locations = config.getAllLocations();
    for (std::map<std::string, std::map<std::string, std::string>>::const_iterator it = locations.begin();
         it != locations.end(); ++it) {
        std::cout << "Location " << it->first << std::endl;
    }

    return 0;
}
```

</details>

<details>
<summary>📋 TODO et notes développement</summary>

## TODO jiminou

### DONE
- [x] vector to map
- [x] ip doesnt start with 0 if size > 0
- [x] utils with map
- [x] validate directive
- [x] duplicate directive ?
- [x] check if folder exist
- [x] add max_size_body (allowed content size max)

### DIDNT DO IT
- [ ] make template for utils (reason: overkill for now)

## Flux d'exécution

```
1. LECTURE
   Fichier.conf → FileReader → vector<string> (lignes)

2. PARSING
   vector<string> → ConfigParser → ServerConfig + LocationsConfig

3. VALIDATION
   ServerConfig → ConfigValidator → ✓/✗ + messages d'erreur

4. STOCKAGE
   Données validées → WebservConfig (std::maps)

5. UTILISATION
   WebservConfig.getPort() → int (valeur prête à utiliser)
```

</details>
