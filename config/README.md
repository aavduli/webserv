# Quick Reference

## Tableau des Directives</summary>

| Directive | Niveau | Comment l'utiliser |
|-----------|---------|-------------------|
| **listen** | serveur | `int port = config.getPort();` |
| **host** | serveur | `std::string host = config.getHost();` |
| **server_name** | serveur | `std::string name = config.getServerName();` |
| **root** | serveur | `std::string root = config.getRoot();` |
| **index** | serveur | `std::string index = config.getIndex();` |
| **allow_methods** | serveur | `std::vector<std::string> methods = config.getAllowedMethods();` |
| **max_size_body** | serveur | `size_t size = config.getMaxBodySize();` |
| **error_page** | serveur | `std::string error = config.getErrorPage(404);` |
| **max_content_length** | serveur | `size_t maxContent = config.getMaxContentLength();` |
| **location_exists** | serveur | `bool exists = config.hasLocation("/api");` |
| **methods** | location | `std::string methods = config.getLocationConfig("/api")["methods"];` |
| **autoindex** | location | `std::string autoindex = config.getLocationConfig("/files")["autoindex"];` |
| **return** | location | `std::string redirect = config.getLocationConfig("/redirect")["return"];` |
| **cgi_ext** | location | `std::string extensions = config.getLocationConfig("/cgi")["cgi_ext"];` |


## ParsingUtils - Fonctions</summary>

| Fonction | Usage | Input | Output | Code |
|----------|-------|-------|--------|------|
| **split()** | Découper string | `("GET POST", ' ')` | `["GET", "POST"]` | `ParsingUtils utils; std::vector<std::string> parts = utils.split(str, ' ');` |
| **trim()** | Nettoyer espaces | `"  value  "` | `"value"` | `ParsingUtils utils; std::string clean = utils.trim(str);` |
| **parseSize()** | Convertir taille | `"1M"` | `1048576` | `ParsingUtils utils; size_t bytes = utils.parseSize("1M");` |
| **isValidIP()** | Valider IP | `"127.0.0.1"` | `true` | `ParsingUtils utils; bool valid = utils.isValidIP(ip);` |
| **isValidPort()** | Valider port | `"8080"` | `true` | `ParsingUtils utils; bool valid = utils.isValidPort(port);` |
| **isValidMethod()** | Valider méthode | `"GET"` | `true` | `ParsingUtils utils; bool valid = utils.isValidMethod(method);` |
| **toLowerCase()** | Minuscules | `"GET"` | `"get"` | `ParsingUtils utils; std::string lower = utils.toLowerCase(str);` |

## Exemples d'utilisation rapide</summary>

### Récupérer config serveur
```cpp
WebservConfig config("config/def.conf");
int port = config.getPort();                           // 8080
std::string host = config.getHost();                   // "127.0.0.1"
std::vector<std::string> methods = config.getAllowedMethods(); // ["GET", "POST"]
size_t maxSize = config.getMaxBodySize();              // 1048576 ressort la valeur en octets
size_t maxContent = config.getMaxContentLength();      // 1048576 limite requête
std::string errorPage = config.getErrorPage(404);      // "/error404.html"
bool hasApi = config.hasLocation("/api");              // true si location existe

```

```cpp
// Pour MessageParser - limite de contenu
size_t maxContent = config.getMaxContentLength();

// Pour MessageHandler - pages d'erreur
std::string page404 = config.getErrorPage(404);  // "./www/errors/404.html"
std::string page403 = config.getErrorPage(403);  // "./www/errors/403.html"
std::string page500 = config.getErrorPage(500);  // "./www/errors/500.html"


// Pour le routing
if (config.hasLocation("/api")) {
    // Traiter requête API
} else {
    // Route par défaut
}


### Récupérer config location
```cpp
std::map<std::string, std::string> apiLocation = config.getLocationConfig("/api");
std::string methods = apiLocation["methods"];          // "GET POST"
std::string autoindex = apiLocation["autoindex"];      // "on" ou "off"
std::string root = apiLocation["root"];                // "/var/www/api"
```

### Utiliser ParsingUtils
```cpp
ParsingUtils utils;

// Split méthodes
std::string methodsStr = "GET POST DELETE";
std::vector<std::string> methodsList = utils.split(methodsStr, ' ');
// methodsList = ["GET", "POST", "DELETE"]

// Parse taille
size_t bytes = utils.parseSize("10M");                 // 10485760

// Validation
bool validIP = utils.isValidIP("127.0.0.1");           // true
bool validMethod = utils.isValidMethod("GET");          // true
```

### Pattern complet d'utilisation
```cpp
#include "config/WebservConfig.hpp"

WebservConfig config("config/mega.conf");

if (!config.isvalid()) {
    std::cerr << config.getLastError() << std::endl;
    return 1;
}

// Serveur
int port = config.getPort();
std::string host = config.getHost();

// Location spécifique
std::map<std::string, std::string> location = config.getLocationConfig("/api");
std::string methods = location["methods"];

// Avec ParsingUtils si besoin de parser
ParsingUtils utils;
std::vector<std::string> methodsList = utils.split(methods, ' ');
```
## Validation automatique incluse</summary>

✅ **IP valides** : Format IPv4 seulement

✅ **Ports valides** : 1-65535

✅ **Méthodes HTTP** : GET, POST, DELETE uniquement

✅ **Tailles** : Conversion K/M/G → octets automatique

✅ **Redirections** : Codes 301, 302 uniquement

✅ **Syntaxe** : Accolades et directives vérifiées

</details>

<details>
<summary># Webserv - config file parser TODO - jroulet</summary>

voir plus bas pour l'utilisation

### DONE
 - [ ] vector to map
 - [ ] ip doesnt start with 0 if size > 0
 - [ ] utils with map
  - [ ] validate directive
 - [ ] duplicate direcvite ?
 - [ ] check if folder exist
 - [ ] utils with map
 - [ ] add max_size_body (allowed content size max) - getter return octet
### DIDNT DID IT
 - [ ] make template for utils :reason, kinda overkill (for now too use template)

### TODO


    `see below for utils`

## Utilisation des std::map

je vais utiliser des maps a la place de vector car:
1) recherche plus rapide
2) key - value -> plus claire dans le code
3) on peut facilement verifier si une directive existe (voir si dessous)
4) possibilité d'ajouté des directive après

### Exemple d'utilisation
#### a modifier dans mon code :
```cpp
// Au lieu de ça :
std::vector<ConfParam> params;

// Utiliser ça :
std::map<std::string, std::string> serverDirectives;
std::map<std::string, std::map<std::string, std::string> > locations;

// Recherche directe :
if (serverDirectives.find("listen") != serverDirectives.end())
    port = serverDirectives["listen"];
```

## utils_parsing.cpp - TODO pour ahanzi - a voir ses besoins

### Fonctions utils

```cpp
// utils_parsing.hpp
class ParsingUtils {
public:
    static std::string trim(const std::string& str); // a modifier
    static std::vector<std::string> split(const std::string& str, char delimiter); // a faire
    static bool isValidIP(const std::string& ip); // a faire
    static bool isValidPort(const std::string& port); // a faire
    static std::string toLowerCase(const std::string& str); // a faire
    static bool isValidMethod(const std::string& method); // a faire
};
```

### utilisation file config + requete http

- **Config parsing** : trim, split, validation IP/port
- **HTTP parsing** : split, toLowerCase, methodes HTTP

### dossier parsign

```
webserv/
├── parsing/
│   ├── utils_parsing.cpp/hpp
│   ├── config_parser.cpp/hpp
│   └── http_parser.cpp/hpp
```

## utilisation de methodes

### `trim(const std::string& str)`
Supprime les espaces, tabs, retours à la ligne au début et fin d'une chaîne. //cf requete HTTP a faire

### `split(const std::string& str, char delimiter)` a voir avec ahanzi
Découpe une chaîne selon un délimiteur. Utile pour :
- Config : `allow_methods GET POST DELETE` → ["GET", "POST", "DELETE"]
- HTTP : `Host: localhost:8080` → ["Host", " localhost:8080"]

### `isValidIP(const std::string& ip)`
Vérifie format IPv4

### `isValidPort(const std::string& port)`
1 à 65535 ???

### `toLowerCase(const std::string& str)`
http est case sensitif

### `isValidMethod(const std::string& method)`
 `allow_methods` POST - GET ...



# overview parsing / validation file config

Le module config parse et valide les fichiers de configuration nginx-like pour webserv.

## Architecture

```
WebservConfig (API principale)
├── ConfigParser (parsing)
├── ConfigValidator (validation)
├── ParsingUtils (utilitaires)
└── FileReader (lecture fichier)
```

## Utilisation simple

```cpp
#include "config/WebservConfig.hpp"

// Charger une config
WebservConfig config("config/def.conf");

if (!config.isvalid()) {
	std::cerr << "Erreur: " << config.getLastError() << std::endl;
	return 1;
}

// Récupérer les valeurs serveur
int port = config.getPort();                    // 8080
std::string host = config.getHost();            // "127.0.0.1"
std::string serverName = config.getServerName(); // "localhost"
std::vector<std::string> methods = config.getAllowedMethods(); // ["GET", "POST"]
```

## API Getters disponibles

### Serveur
```cpp
int getPort() const;                          // Parse "listen 8080" → 8080
std::string getHost() const;                  // Parse "host 127.0.0.1"
std::string getServerName() const;           // Parse "server_name localhost"
std::string getRoot() const;                 // Parse "root /var/www"
std::string getIndex() const;                // Parse "index index.html"
size_t getMaxBodySize() const;               // Parse "max_size_body 1M" → 1048576
std::vector<std::string> getAllowedMethods() const; // Parse "allow_methods GET POST"
std::string getErrorPage(int code) const;       // Parse "error_page 404 /404.html" → "/404.html"
```

### Locations
```cpp
// Toutes les locations
const std::map<std::string, std::map<std::string, std::string>>& getAllLocations() const;

// Location spécifique
std::map<std::string, std::string> getLocationConfig(const std::string& path) const;
```

### Utilitaires
```cpp
bool isvalid() const;                        // Config valide ?
std::string getLastError() const;           // Dernière erreur
void printConfig() const;                   // Debug print
```

## Format de fichier config supporté

```nginx
server {
	listen 8080;                             # Port
	host 127.0.0.1;                         # IP
	server_name localhost;                   # Nom serveur
	root /var/www/html;                      # Dossier racine
	index index.html;                        # Page par défaut
	allow_methods GET POST DELETE;           # Méthodes HTTP
	max_size_body 1M;                        # Taille max body
	error_page 404 /error404.html;           # Pages d'erreur spécifiques
	error_page 403 /error403.html;           # Plusieurs codes supportés
	error_page 500 /error500.html;           # Format: code filepath


	location /api {                          # Bloc location
		methods GET POST;                    # Méthodes pour cette route
		root /var/www/api;                   # Root spécifique
		autoindex off;                       # Listing dossier
	}

	location /redirect {
		return 301 https://google.com;       # Redirection
	}
}
```

## Validation

- **IP valides** : Format IPv4 (ex: 127.0.0.1)
- **Ports valides** : 1-65535
- **Méthodes HTTP** : GET, POST, DELETE
- **Codes redirection** : 301, 302
- **Tailles** : Conversion automatique K/M/G → octets
- **Fichiers** : Vérification existence
- **Syntaxe** : Accolades, directives

## Gestion d'erreurs

```cpp
WebservConfig config("invalid.conf");

if (!config.isvalid()) {
	// Récupérer l'erreur détaillée
	std::string error = config.getLastError();
	std::cerr << "Config error: " << error << std::endl;
}
```

## Exemple complet

```cpp
#include "config/WebservConfig.hpp"

int main() {
	WebservConfig config("config/mega.conf");

	if (!config.isvalid()) {
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
        const std::map<std::string, Location>& locations = config.getAllLocations();
        for (std::map<std::string, Location>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
            std::cout << "Location " << it->first << std::endl;
        }

	return 0;
}
```

# Architecture jiminou

## 1. Vue d'ensemble des fichiers

```
config/
├── WebservConfig.hpp/cpp        ← API PRINCIPALE (interface publique)
├── ConfigParser.hpp/cpp         ← Parse le fichier texte → structs
├── ConfigValidator.hpp/cpp      ← Valide les données parsées
├── ParsingUtils.hpp/cpp         ← Utilitaires (split, trim, validation)
├── FileReader.hpp/cpp           ← Lit les fichiers ligne par ligne
├── ConfigData.hpp               ← Structures de données (ServerConfig, LocationConfig)
└── *.conf                       ← Fichiers de configuration
```

## 2. Flux d'exécution

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

## 3. Diagramme détaillé

```
[Fichier def.conf]
       ↓
[FileReader::readLines()]
       ↓
[vector<string> lines]
       ↓
[ConfigParser::parseServer()]     [ConfigParser::parseLocations()]
       ↓                                  ↓
[ServerConfig]                     [LocationsConfig]
       ↓                                  ↓
[ConfigValidator::validateServerConfig()] [ConfigValidator::validateLocationConfig()]
       ↓                                  ↓
[Validation OK/KO]
       ↓
[WebservConfig]
       ↓
[getters  → pour bebou]
```

## 4. Rôle de chaque fichier

### **WebservConfig**
```cpp
// lance tout le processus
WebservConfig config("def.conf");
↓
1. FileReader lit le fichier
2. ConfigParser parse les lignes
3. ConfigValidator valide les données
4. Stockage dans maps internes
5. API getters pour les collègues
```

### **ConfigParser** (valide la syntaxte)
```cpp
// text to struct
"listen 8080;" → ServerConfig.directives["listen"] = "8080"
"location /api {" → LocationConfig{path="/api", directives={...}}
```

### **ConfigValidator** (validation des donnees)
```cpp
// Vérifie cohérence des données
"8080" → isValidPort() → true/false
"127.0.0.1" → isValidIP() → true/false
"GET POST" → isValidMethod() → true/false
```

### **ParsingUtils**
```cpp
// Fonctions réutilisables
"GET POST DELETE" → split(' ') → ["GET", "POST", "DELETE"]
"  listen 8080;  " → trim() → "listen 8080;"
"1M" → parseSize() → 1048576
```

### **FileReader**
```cpp
// Lit et nettoie les lignes
def.conf → ["server{", "listen 8080;", "}", ...]
```

### **ConfigData**
```cpp
// Définit les formats de stockage
struct ServerConfig { map<string, string> directives; }
struct LocationConfig { string path; map<string, string> directives; }
```

## 5. schema

```
WebservConfig fait :
   def.conf → FileReader → ["server{", "listen 8080;", ...]
              ↓
   ConfigParser → ServerConfig{directives["listen"]="8080"}
              ↓
   ConfigValidator → Vérification port 8080 valide ✓
              ↓
   Stockage → _server["listen"] = "8080"

```

</details>
