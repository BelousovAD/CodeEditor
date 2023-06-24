#pragma once
#include <wx/defs.h>

enum {
    ID_MARGIN_LINENUMBER = 0,
    ID_MARGIN_DIVIDER,
    ID_MainWindow = wxID_HIGHEST + 1,
    ID_Notebook,
    ID_Editor,
    ID_ProjectExplorer,
    ID_ImportsList,
    ID_ObjectExplorerWindow,
    ID_HelpWindow,
    ID_ObjectExplorer,
    ID_Help,
    ID_Console,
    ID_HIGHLIGHTFIRST,
    ID_HIGHLIGHTLAST = ID_HIGHLIGHTFIRST + 30,
    ID_ProjectNew,
    ID_ProjectOpen,
    ID_ProjectSave,
    ID_ObjectExplorerOpen,
    ID_ProjectClose,
    ID_New,
    ID_Save,
    ID_IncludeFile,
    ID_Open,
    ID_ExcludeFile,
    ID_SetFileAsMain,
    ID_FileUp,
    ID_FileDown,
    ID_Delete,
    ID_DirAppend,
    ID_DirUp,
    ID_DirDown,
    ID_DirDelete,
    ID_OpenImportsList,
    ID_Compile,
    ID_Build,
    ID_Execute,
    ID_TargetMIN,
    ID_TargetMAX = ID_TargetMIN + 3,
};

const std::vector<wxString> Targets = {
    "Win32 Console Application(MS Visual C++ 6.0)",
    "Win32 Application (MS Visual C++ 6.0)",
    "Console Application (MinGW 3.1.0)",
    "Win32 Application(MinGW 3.1.0)"
};

const wxString UserManual = "Создать проект:\n"
"1) Проект->Новый проект;\n"
"2) В диалоговом окне выбрать необходимую директорию и нажать на кнопку «Открыть»;\n"
"3) Приложение отобразит файлы проекта в обозревателе проекта;\n"
"4) Приложение добавит к наименованию главного окна название проекта.\n\n"

"Открыть проект :\n"
"1) Проект->Открыть проект;\n"
"2) В диалоговом окне выбрать необходимый файл проекта и нажать на кнопку «Открыть»;\n"
"3) Приложение отобразит файлы проекта в обозревателе проекта;\n"
"4) Приложение добавит к наименованию главного окна название проекта.\n\n"

"Сохранить проект :\n"
"1) Проект->Сохранить проект;\n"
"2) Приложение сохранит проект и снимет отметку символом «звёздочка» с названия проекта в наименовании главного окна.\n\n"

"Закрыть проект.\n"
"1) Проект->Закрыть проект;\n"
"2) Закрываются все открытые файлы(можно сохранить их во время закрытия);\n"
"3) Приложение очистит виджет «Обозреватель проекта»;\n"
"4) Приложение удалит название проекта из наименования главного окна(если название проекта отмечено символом «звёздочка», то будет предлоено сохранить проект).\n\n"

"Открыть «Обозреватель объектов» :\n"
"1) Проект->Открыть «Обозреватель объектов»;\n"
"2) Отобразится модальное окно со списком объектов, специфичных для языка О2М;\n\n"

"Создать файл :\n"
"1) Файл->Новый;\n"
"2) В диалоговом окне написать название нового файла и нажать на кнопку «Сохранить»;\n"
"3) Приложение отобразит файл в обозревателе проекта;\n"
"4) Приложение отметит название проекта в наименовании главного окна символом «звёздочка».\n\n"

"Подключить файл :\n"
"1) Файл->Подключить;\n"
"2) В диалоговом окне выбрать необходимый файл и нажать на кнопку «Открыть»(если файл уже был добавлен, то отобразится сообщение об этом);\n"
"3) Приложение отобразит файл в обозревателе проекта;\n"
"4) Приложение отметит название проекта в наименовании главного окна символом «звёздочка».\n\n"

"Открыть файл :\n"
"1) Обозреватель проекта->ПКМ по файлу->Открыть;\n"
"2) Приложение выведет в главное окно новую вкладку с файлом или переключится на существующую вкладку, если файл уже открыт.\n\n"

"Назначить файл главным в проекте :\n"
"1) Обозреватель проекта->ПКМ по файлу->Назначить главным;\n"
"2) Приложение подсветит название файла в обозревателе проекта красным цветом на сером фоне;\n"
"3) Приложение отметит название проекта в наименовании главного окна символом «звёздочка».\n\n"

"Cохранить файл :\n"
"1) Файл->Сохранить;\n"
"2) Приложение сохранит файл и снимет отметку символом «звёздочка» с вкладки.\n\n"

"Закрыть файл :\n"
"1) Нажать на символ «крестик» на вкладке;\n"
"2) Если вкладка отмечена символом «звёздочка», то предложится сохранить файл;\n"
"3) Приложение закроет вкладку.\n\n"

"Исключить файл:\n"
"1) Обозреватель проекта->ПКМ по файлу->Исключить;\n"
"2) Приложение отобразит запрос на подтверждение действия;\n"
"3) Приложение удалит файл из обозревателя проекта, если действие будет подтверждено;\n"
"4) Приложение отметит название проекта в наименовании главного окна символом «звёздочка».\n\n"

"Удалить файл:\n"
"1) Обозреватель проекта->ПКМ по файлу->Удалить;\n"
"2) Приложение отобразит запрос на подтверждение действия;\n"
"3) Приложение удалит файл из обозревателя проекта и из файловой системы, если действие будет подтверждено;\n"
"4) Приложение отметит название проекта в наименовании главного окна символом «звёздочка».\n\n"

"Выбрать язык для подсветки синтаксиса:\n"
"1) Подсветка языка->Выбрать язык;\n"
"2) Приложение подсветит в тексте вкладки синтаксис выбранного языка.\n\n"

"Добавить путь к модулям:\n"
"1) Настройки->Добавить путь к модулям;\n"
"2) В диалоговом окне выбрать необходимую директорию и нажать на кнопку «Открыть»(если директория уже была добавлена, то отобразится сообщение об этом);\n"
"3) Приложение отобразит директорию в списке подключаемых директорий.\n\n"

"Удалить путь к модулям :\n"
"1) Список подключаемых директорий->ПКМ по директории->Удалить;\n"
"2) Приложение удалит директорию из списка подключаемых директорий.\n\n"

"Скомпилировать код проекта :\n"
"1) Запуск->Скомпилировать;\n"
"2) Приложение запустит транслятор «O2M» и выведет его отчёт в виджет «Консоль»;\n"
"3) Приложение запустит компоновщик «Link2M» и выведет его отчёт в виджет «Консоль»;\n"
"4) Приложение запустит генератор проекта для C++ «Make2M» и выведет его отчёт в виджет «Консоль».\n\n"

"Собрать проект :\n"
"1) Запуск->Собрать;\n"
"2) Приложение запустит утилиту «nmake» и выведет её отчёт в виджет «Консоль».\n\n"

"Запустить проект :\n"
"1) Запуск->Запустить;\n"
"2) Приложение запустит проект.\n";