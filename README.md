# Remote covert defensive operations (RCDO)
Random piece of description :>

Menu Strings are easily changable just CTRL+F and search the string

This is the part of the program to add your functions to use in the Functions menu

void functionMenu(uint8_t selection)
{
    if (selection == 1)
    {
        char buffer[20];
        strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[functionMenuIndex].strings[selection]))); //copy function name to display in the function's page
        functionView(0, &test, buffer, simpleRandGen); // follow this convention to call your functions the last args is the function name!
    }
}
