### Auteur : 
Abderrahmane BELLA
# Bienvenue dans le readme du projet Qtree

le projet est décomposé en deux sections majeures, le compresseur et le décompresseur. Chacun possédant leur propre Makfile, exécutable, headers, fichiers sources et main. Respectivement contenues dans les dossiers, executable, header, main, source.

La section du compresseur étant elle-même contenue dans le dossier compressor, quand à celle du décompresseur dans le dossier decompressor Pour les compiler il suffit de se placer à la racine du dossier correspondant et utiliser le makefile inclus. Après quoi l'éxécutable sera généré dans le dossier prévu à cet effet, et éxécuter cet éxécutable avec diverses options, comme par exemple, la variance. Un exemple de ligne de commande type d'utilisation serait : 

- Pour le compresseur : "./compressor <nom_de_votre_fichier>.pgm -a 5.0 -b 2.0"

- Pour le décompresseur : "./decompressor <nom_de_votre_fichier>.qtc"

Ces commandes vous produiront respectivement deux fichiers, la première placera le fichier qtc résultant de la compression dans le dossier mycompressions contenu dans le dossier executable, la seconde placera l'image restauré dans le dossier executable.

Vous trouverez également dans le cas où vous nécessitez de décompressez multiples QTC un script bash pour la décompression, pour l'utiliser, il vous suffit de placer un executable du décompresseur, s'il n'y en a pas déjà un, dans le dossier QTC.lossless, une fois ceci fais, il suffit de vous placer à la racine et d'éxécuter "bash test.sh" afin de décompresser tout les qtc contenus dans QTC.lossless. Les résultats de la décompression sera situé dans le dossier mydecompression dans lequel vous trouverez tout les pgm restaurés.




Merci de votre attention.