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
        functionView(0, simpleRandGen); // follow this convention to call your functions the last args is the function name!
    }
}
```

Add on to the cases for specific messages you want to print during execution
```
void printStatus(int x) //call this to print messages after execution
{
  displayBuffer.fontColor(defaultFontColor, defaultFontBG);

  switch(x){
    case(0): //add to the cases to display message on function completetion
        printCenteredAt(menuTextY[3], "Plug In The USB!");
        break;
    case(1):
        printCenteredAt(menuTextY[3], "Done!");
        break;
  }
}
```
