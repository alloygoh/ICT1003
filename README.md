# Remote covert defensive operations (RCDO)
Random piece of description :>

To compile open the menu_main.ino then compile

Menu Strings are easily changable just CTRL+F and search the string

This is the part of the program to add your functions to use in the Functions menu

```
void functionMenu(uint8_t selection)
{
    if (selection == 1)
    {
        char buffer[20];
        
        //copy function name to display in the function's page
        strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[functionMenuIndex].strings[selection]))); 
        
        // follow this convention to call your functions the last args is the function name!
        functionView(0, &test, buffer, simpleRandGen); 
    }
}
```
