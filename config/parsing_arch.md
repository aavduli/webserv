# Webserv - config file parser TODO - jroulet

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