# Documentations codes

Le dossier **/Documentation codes** contient les fichiers de documentation de code réalisé pendant le semestre.
L'idée de ces documentations est d'aider à la compréhension, au-delà de ce que la simple lecture permet de déceler.
Parmis tous les fichiers, il faut regarder 2 documents :

* **stm32_code.html** traite du code utilisé sur la STM32. Elle est découpée selon les modules (fichiers en .c) utilisés sur la carte. Compte tenu de la complexité du langage C, lire et relire les passages est pertinent pour ne pas déregler le code. **Il est conseillé de changer au minimum le code** pour ne pas rendre le code non maintenable.
Pour mettre à jour la documentation, il faut convertir le document en .md en .html.

* **stm32_***com***.html** traite du code en python côté PC. L'objectif est de commander la carte STM32 tout en lisant les données renvoyées par la carte. Une bonne partie n'est pas destinée à la modification.

Il ne faut supprimer aucun fichier pour permettre un affichage propre des documentations générées, comme par ex index.js ou autre.