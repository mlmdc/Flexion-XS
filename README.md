# FLEXION XS - DEV

Ce git est destiné au développement du projet de réglette en flexion du module CMV, pour le semestre 2024P.

## Reprise du projet

Les documents dans ce dossier rendent compte du travail technique effectué pendant le semestre 2024P. La partie mécanique vibratoire, coeur de ce projet, est abordée dans le cours de mécanique de M. Helbert.
Nous vous proposons ce plan pour reprendre et comprendre ce projet de **Flexion XS**:

1. Aller dans **/0_Documents** et lire le tutoriel. Ce dernier explique comment effectuer une mesure en partant de rien.
2. En parallèle du tutoriel, regardez les vidéos dans **/4_Medias/Videos**.
3. Essayez de faire le lien entre les vidéos et le tutoriel écrit. En effet, associer le résultat avec l'explication vous permettra d'associer les principes de la maquette (conception, code, ect..) tel que nous l'avons formulé dans ce dossier.
4. Essayez d'effectuer une mesure (mesure en régime libre/relacher la reglette est de loin le + simple) avec le code **serial_com.ipynb** et le projet STM32CubeIDE **/REGLETTE_Workspace**.
Les problèmes d'installations vont alors apparaître. **N'essayez pas de changer quelque chose dans le code ou sur la maquette avant d'arriver à mesurer, vous ne feriez que vous tirer une balle dans le pied pour la suite du semestre**. Réparez les erreurs vous permettra d'apprendre le projet.

A partir du moment où la mesure est réussie, il est temps de passer à l'évolution du projet:

5. Découvrez les choix de conceptions effectués dans **/Documents/2_listes_des_taches.pdf**, dans **/1_Documentation_codes**, dans **/2_Conception_electronique** et dans **/3_Conception_Mecanique**.

6. Nous avons pensé aux évolutions possibles dans le document *3_Perspectives.pdf*. En conjonction avec *erreurs_projet.ods*, cela vous donnera les pistes pour faire évoluer le projet, tant dans la résolution de problèmes que dans l'amélioration continue.

7. Le plus gros du travail à améliorer est dans le post-traitement. Les analyses que nous avons pu faire on été pour nous corrects, mais se sont en fait révélés insuffisants pour des calculs plutôt précis.

Documents pour aider :

* **/Medias/Photos** fourre-tout de photos. Des photos de la maquette en état de fonctionnement, des photos de courbes et autres, ces médias sont là pour aider à la compréhension.

* **/2_Conception_electronique** contient les fichiers qui ont été réalisés dans la partie électronique mais ne traite pas de la partie intégration physique.

* **/3_Conception_Mecanique** contient les fichiers de CAO (assemblage et pièces, fichiers d'impressions 3D).

## Organisation

Le git, ainsi que les documents, sont organisés selon leurs fonctions.

### Documentations

Le dossier **/Documentation** contient les fichiers de documentation de code réalisé pendant le semestre.
L'idée de ces documentations est d'aider à la compréhension, au-delà de ce que la simple lecture permet de déceler.
Parmis tous les fichiers, il faut regarder 2 documents :

* **stm32_code.html** traite du code utilisé sur la STM32. Elle est découpée selon les modules (fichiers en .c) utilisés sur la carte. Compte tenu de la complexité du langage C, lire et relire les passages est pertinent pour ne pas déregler le code. **Il est conseillé de changer au minimum le code** pour ne pas rendre le code non maintenable.
Pour mettre à jour la documentation, il faut convertir le document en .md en .html.

* **stm32_***com***.html** traite du code en python côté PC. L'objectif est de commander la carte STM32 tout en lisant les données renvoyées par la carte. Une bonne partie n'est pas destinée à la modification.

### Documents

**/Documents** contient les documents comme le tutoriel et autres. La source des documents (en .md) ainsi que les images utilisées sont aussi fournies, dans le but de pouvoir modifier les documents pendant les autres semestres.

**/Sources** contient les sources des documents de Tutoriel.pdf et Perspectives.pdf, ainsi que les images sources utilisées pour agrémenter visuellement les documents.
Si vous souhaitez produire un document, mettez vous dans ce répertoire et générez par commande les .pdf.
L'idée de séparer le produit d'arrivée de sa source est pour ne pas mélanger les travaux déjà effectués de ceux qui n'ont pas été encore effectués.

### Measurements

Le dossier **/Measurements** est destiné au partage entre notebooks des mesures.
**Ne pas modifier le nom du dossier sans prendre en compte la modification dans le code**.
**Ne pas supprimer le répertoire si vous souhaitez faire du post-traitement**.

### Post-traitements

**/Post-traitements** contient les notebooks de post-traitements, avec un rangement par type de mesures.
Au lieu de versionnage par git, le versionnage est fait en copiant les documents et en les modifiant.
L'objectif est de garder les précedentes modifications pour réutilisations ultérieures.

### Medias

Ce répertoire contient les médias photos et vidéos qui vous permettront de vous situer et de comprendre le projet.

**/Photos**, comme son nom l'indique, est le lieu de toutes les photos. Celles-ci vont de l'illustration de diaporama à des photos de la maquette.

**/Videos** est lui pour les vidéos tutoriels.

### REGLETTE_Workspace

Dossier workspace pour STM32CubeIDE. Ne modifier que dans STM32CubeIDE.

### Conception mécanique

Ce répertoire contient :

* /"Impression 3D" contient les fichiers .stl réalisés, par exemple le support d'acceleromètre.

* /"LETTRE 7 ET 8" contient les .CATPart (sans assemblage) des lettres 7 & 8 utilisées pour le nouveau balisage.

* / a l'assemblage de la maquette ainsi que les parties utilisées. En parallèle, la caractérisation du moteur et de son filtre est donné.

Un pdf d'explications vis-à-vis du procédé de conception est donné.

# Conception électronique

Ce dossier contient le travail effectué sur la conception électronique ajoutée au projet. 
Cela correspond à 2 parties :
* La remise au gout du jour et de la réalité du schéma de sécurité EDITSAB
* La conception de la carte filtre passe-bas

A ces 2 taches sont ajoutés des rapports d'activités expliquant le processus de conception.

### .git

Fichier spécifique à git, ne pas modifier sans ```$ git ...```.

### /..

La racine du dossier, où se trouve tous les autres documents. 
Contient :

* **serial_com.ipynb** jupyter notebook pour la communication depuis le PC.

* **.gitignore** document pour git.


## Versionnage

STM32CubeIDE : 1.12.0
STM32CubeIDE/CubeMX : 6.8.7

Python : 3.10

Pour installer tous les packages nécessaires :
```bash
pip install 'Documentation codes'/requirements.txt
```