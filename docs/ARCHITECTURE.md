# Architecture des Fichiers

## Structure du repo

Le repo comporte le code source de **erraid** et de **tadmor**, ainsi que
les fichiers suivant :

- README.md : Des informations de base pour le projet
- AUTHOR.md : Les developeurs du projets
- code/ : Le code source
- docs/ : La documentation de l'architecture, potentiellement une config Doxygen

## Fichiers source

Les choix d'architecture suivant s'appliquent pour le code **d'erraid** et de **tadmor**

Les fichiers source (`.c` et `.h`) vivent `src/` et `include/`, respectivement.

On a opté pour que les 2 arborescences soient symmetriques, donc tout fichier
`src/.../f.c` aient sa contrepartie `include/.../f.h` avec `...` étant le
meme chemin.

Donc : 

```bash
$ tree src/

src
├── a
│   └── b
│       └── c
│           └── f1.c
└── d
    └── f2.c
```

```bash
$ tree include/

include/
├── a
│   └── b
│       └── c
│           └── f1.c
└── d
    └── f2.c
```


