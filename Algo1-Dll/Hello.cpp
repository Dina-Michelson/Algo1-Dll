#include <stdio.h>
#include <iostream>
#include "pch.h"
#include <iostream>
#include <string.h>
#include <fstream>
#include<map>
#include <vector>
#include <string.h>
#include <sstream>
//#include <bits/stdc++.h>
#include <algorithm>
#include <list>

using namespace std;

/*extern "C"
{
    __declspec(dllexport) void DisplayHelloFromDLL2()
    {
        printf("Hello from DLL !\n");
    }
}*/

extern "C" __declspec(dllexport) void DisplayHelloFromDLL2() {
    printf("Hello from Algo Dll!!!\n");
}