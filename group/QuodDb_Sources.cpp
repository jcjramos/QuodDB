#define DllEntryPoint

#error
/*
TO DO LIST:
- Limite de 1000 registos
- Novo GUI
- Funcoes de Navegação mais fixes
- Progress Bar ao importar dados

Bugs:
1. In "Data Edition", I canot find a way to enter a"." (full stop) in a
field of type "real number" when adding a new record.The "*" key only
produces an asterisk. Keeping it pressed normally brings up the Symbol
table in other applications, but not in QuodDB. Could the field type be
wrong, as the keypad behaves like when you enter a phone number? If you
were allowed to change the text input method from numeric to text using
the "#" key that would perhaps be better.

2. In "Data Report", when trying to create a new filter on a date field,
only a time field appears after I select the operation (Equal, Less than,
etc.). The same happens when I try to do a lookup on a date field.

Other:
- Double occurence of the default DB in DB list...
- How to enter a decimal point when a fields type is number (real) ?
- Why is the options entry available if there are no options to set (except
for lists)????
- Ability to save the header field is gone
- Import still crashes under the following circumstances:
     Import of a .csv file which is generazed by FileMaker Pro immediately
     crashes the app

     Import of a .txt file which has a tab as delimiter cuts off the first
     character of the first field (i. e. a text field)
- The database file gets bigger and bigger with every action


*/
