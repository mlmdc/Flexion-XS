# Perspectives d'évolutions

## Post-traitements

Le travail de post-traitement des données mesurées doit représenter une grande partie du travail qui suit celui que nous avons fourni. C'est le point le plus faible de notre travail, principalement par manque de temps et d'une concentration sur les aspects fondamentaux du projet (remise en état, code, etc..). Nous vons proposons :

1. Déterminer le modèle mathématique/mécanique de la réglette, puis le faire vérifier avec M. Contal et M. Helbert.

2. Simuler la réglette dans les deux cas qui nous intéressent, en relâchement et en oscillation forcée.
L'objectif est d'avoir un modèle théorique qui va plus loin que le cas parfait (sans poids ajoutés) sur lequel des comparaisons pourront être faites.

3. Retravailler les scripts de post-traitement pour améliorer la qualité et la répétabilité des résultats.


**ATTENTION**, ne faites pas l'erreur d'intégrer le post-traitement dans l'IHM. Ce n'est ni pratique à l'usage, ni le but d'une IHM. Le plus simple et le plus modulaire est l'approche par Jupyter Notebook, qui permet d'intégrer facilement des portions de code supplémentaires dans le flow du traitement, tout en permettant de réflechir et expérimenter sans tout casser.


## Acceleromètre
L'acceleromètre est une des pièces les plus importantes du dispositif.
Plusieurs améliorations sont à penser :

1. Passer de 1 à 3 acceleromètres en parallèles

2. Passer d'acceleromètres analogiques a I2C

3. Intégrer le ou les filtres dans la carte de support jaune

## Carte Jaune
La carte de support jaune permet le lien de beaucoup des composants pour l'acquisition finale. Malheureuseument, la conception à date (S2024P) laisse beaucoup à désirer et de nombreux problèmes, de masses notamment, rendent l'usage de la maquette très peu simple et alambiquée.
Nous proposons les changements de conceptions suivants pour rendre le projet à la fois plus simple à continuer et à utiliser :

1. Séparer correctement les masses d'acquisitions et de puissance

2. Refaire le cablâge de la partie sécurité pour le rendre propre et modifiable

3. Mettre des diodes pour bloquer les nombreux appels de courant et séparer les masses

4. Privilégier une ou deux surfaces de la carte pour les connecteurs 

5. Refaire les indications sur la carte (fausses pour l'instant)

6. Intégrer les filtrages du capteur

## Electroaimant 
L'electroaimant permet de répétabiliser les essais. Il n'est pas pour l'instant fiable dù à une connexion sans doute faible. Nous proposons les choses suivantes

1. Refaire le support pour accomoder plus de longueur

2. Refaire le câble entre le connecteur et l'aimant

3. Vérifier, voir refaire le connecteur

## STM32
A priori peu de choses sont à réaliser sur la carte de traitement, cependant des travaux d'adaptations sont sans doutes à faire pour permettre les autres tâches :

1. Passer de 1 à 3 channel en ADC DMA, avec une fréquence d'échantillonage plus rapide

2. Transmettre plus de données par UART

3. Ne transmettre plus que les accelerations traitées

4. Régler le problème de connexion instable et d'ordres non respectés

5. Passer d'une transmission de ligne en ligne à une transmission de tableaux pour augmenter la fréquence d'échantillonage et capter ainsi plus de fréquences.

6. **Tests unitaires** pour les différentes sous-sections du code. Dans le code fourni, il s'agit a priori de rajouter uniquement des fonctions qui teste les aspects.

## IHM
Pour des raisons de temps et de gestion de projet, l'ancienne IHM n'a pas pu être développée.
Il faut donc réaliser une nouvelle IHM dont les outils à votre disposition sont :

- Le code STM32 avec les identifiants des données de par et d'autre du câble

- Du code python avec de la documentation

Les nécessaires sur l'IHM :

- L'affichage de la vitesse du moteur

- L'affichage des accélérations sous forme de graphe

- Les commandes de l'électroaimant et du moteur, notamment dans le déclenchement et les valeurs de réglages du PID par exemple

- L'état des capteurs

## Oscillations harmoniques
Lors de notre développement, nous avons pu développer l'étude du régime libre et essayer celle du régime forcée. Nous proposons :

1. De trouver un moyen de monter proprement l'excentrique sur le moteur **à vide** pour permettre des grandes vitesses de rotations

2. Finir le code sur l'aspect harmonique, notamment pour la mesure et le contrôle sur une fréquence précise

3. Post-traitement et calculs

4. Déterminer les bonnes pratiques de mesure pour ce régime particulier

## Régime libre
Il reste encore un peu de travail, par exemple :

1. Trouver le 3ième mode

2. Mieux traiter les données, pour peut-être réussir à utiliser le circle fitting et mieux trouver les modes.
