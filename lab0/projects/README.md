#  Lab 0 – Operating Systems 20261

## 
Aprender los conceptos y las herramientas básicas para mejorar su interacción con la terminal shell de linux (Bash o zsh)
---

## Paso a Paso del Reto

###  Creación de la estructura inicial

```bash
mkdir -p operating-systems-20261/laboratories/lab0
cd operating-systems-20261/laboratories/lab0/
pwd > path.txt
```
### Navegar hasta el directorio recién creado.

```bash
cd operating-systems-20261/laboratories/lab0/
```
### Imprimir la ruta absoluta del directorio y luego enviarla al archivo 
### ~/operating-systems-20261/laboratories/lab0/path.txt

```bash
pwd > path.txt
```
### Con un solo comando crear los directorios

```bash
mkdir example music photos projects
```
### crear archivos en cada carpeta nueva creada
utilizo el comando touch file{1..100} para creacion masiva de archivos
y ciclo for para aplicarlo en todas las carpteas 

```bash
for dir in example music photos projects;
do
    touch "$dir"/file{1..100}
done
```

### Por cada uno de los directorios, borra los primeros 10 archivos y los últimos 20.
se utiliza el comando rm file{1..9} file{79..100} para la eliminación parcial de archivos 
```bash
for dir in example photos projects music; 
do 
rm "$dir"/file{1..9} "$dir"/file{81..100}; 
done
```

### A la carpeta projects mueve los directorios: example, music y photos.
```bash
mv example music photos projects/
```

### Eliminación de archivos en projects con modo verboso
```bash
rm -v projects/file{10..80} > output.txt
```
