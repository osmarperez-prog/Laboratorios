# Sumar n números
n = int(input("cuantos numeros deseas sumar "))
total = 0
for i in range(n):
    num = float(input("ingresa un numero "))
    total = total + num

print("la suma total es:", total)

#Invertir número
num1 = input("Ingresa un número entero: ")
invertido = 0
numero = int(num1)
while numero > 0:
    dígito = numero % 10
    invertido = (invertido * 10) + dígito
    numero = numero // 10

print("El numero invertido es:", invertido)
#Información de usuario
nombre = input("Ingresa tu nombre: ")
edad = input("Ingresa tu edad: ")
profesion = input("Ingresa tu profesion: ")
print("Hola " + nombre + ", tienes " + edad + " años y trabajas como " + profesion + ".")