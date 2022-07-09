from pandas import read_excel, ExcelWriter
from tkinter import ttk, StringVar
from tkinter.messagebox import showerror, showinfo
from ttkthemes import ThemedTk
from random import choice


def Start():
    global G_guessing_word, G_answer, G_last_guessing_word, G_last_answer
    G_last_guessing_word, G_last_answer = "", ""
    res_lab.config(text="")
    menu_frame.pack_forget()
    game_frame.pack()
    if cb_get.get() == G_languages[0]:
        G_guessing_word = choice(list(G_keywords.values()))
        for key, val in G_keywords.items():
            if val == G_guessing_word:
                G_answer = key
                break
    else:
        G_guessing_word = choice(list(G_keywords.keys()))
        G_answer = G_keywords[G_guessing_word]
    lab2.config(text=f"Напишіть переклад:\n{G_guessing_word}")


def EnterPressed(event):
    global G_guessing_word, G_answer, G_last_guessing_word, G_last_answer
    G_last_guessing_word, G_last_answer = G_guessing_word, G_answer
    my_answer = entry1.get()
    if my_answer in G_answer.split(","):
        res_lab.config(text=f"Правильно, {G_guessing_word}\n це {G_answer}!", foreground="lime")
    else:
        res_lab.config(text=f"Неправильно, {G_guessing_word}\n це {G_answer}", foreground="red")
    if cb_get.get() == G_languages[0]:
        G_guessing_word = choice(list(G_keywords.values()))
        for key, val in G_keywords.items():
            if val == G_guessing_word:
                G_answer = key
                break
    else:
        G_guessing_word = choice(list(G_keywords.keys()))
        G_answer = G_keywords[G_guessing_word]
    entry1.delete(0, "end")
    lab2.config(text=f"Напишіть переклад:\n{G_guessing_word}")


def AddWords():
    global G_document
    uaword, engword = entry2.get(), entry3.get()
    if uaword == "" or engword == "":
        showerror("Помилка", "Комірки не можуть бути\nз пустими значеннями!")
    else:
        G_keywords[uaword] = engword
        entry2.delete(0, "end")
        entry3.delete(0, "end")
        showinfo("Успіх!", f"Нове слово'{uaword}'було\nдодано з перекладом!")
        G_document = G_document.append({"UaKeywords": uaword, "EngKeywords": engword}, ignore_index=True)
        G_document.to_excel(G_ADDRESS, sheet_name=G_SHEET_NAME)


def OpenEditFrame():
    if G_last_answer != "" and G_last_guessing_word != "":
        game_frame.pack_forget()
        edit_frame.pack()
        edit_entry1.delete(0, "end")
        edit_entry2.delete(0, "end")
        edit_entry1.insert(0, G_last_guessing_word)
        edit_entry2.insert(0, G_last_answer)
    else:
        showerror("Помилка", "Неможливо редагувати слова!")


def SaveEditedWords():
    global G_document, G_last_answer, G_last_guessing_word
    last_guessing_word, last_answer = edit_entry1.get(), edit_entry2.get()
    if last_guessing_word != "" and last_answer != "":
        if cb_get.get() == G_languages[0]:
            uaword, engword = last_answer, last_guessing_word
            G_keywords.pop(G_last_answer)
            G_document = G_document.replace(G_last_answer, uaword)
            G_document = G_document.replace(G_last_guessing_word, engword)
            G_last_answer = uaword
        else:
            uaword, engword = last_guessing_word, last_answer
            G_keywords.pop(G_last_guessing_word)
            G_document = G_document.replace(G_last_answer, uaword)
            G_document = G_document.replace(G_last_guessing_word, engword)
            G_last_guessing_word = uaword
        G_document.to_excel(G_ADDRESS, sheet_name=G_SHEET_NAME)
        G_keywords[uaword] = engword
        showinfo("Успіх!", f'Слова були редаговані на\n"{uaword}", "{engword}"')
    else:
        showerror("Помилка", "Комірки не можуть бути\nз пустими значеннями!")


def ClearWords():
    pass
    # global G_document
    # if cb_get.get() == G_languages[0]:
    #     G_document = G_document[G_document.UaKeywords != G_last_answer]
    # else:
    #     G_document = G_document[G_document.UaKeywords != G_last_guessing_word]
    # G_document.to_excel(G_ADDRESS, sheet_name=G_SHEET_NAME)


if __name__ == "__main__":
    G_guessing_word = ""
    G_last_guessing_word = ""
    G_answer = ""
    G_last_answer = ""
    G_ADDRESS = "EnglishWords.xlsx"
    G_SHEET_NAME = "Sheet1"
    try:
        G_document = read_excel(G_ADDRESS, sheet_name=G_SHEET_NAME)
    except FileNotFoundError:
        showerror("Помилка", f"Файл {G_ADDRESS} не був знайдений!")
    except ValueError:
        showerror("Помилка", f"Лист {G_SHEET_NAME} не був знайдений!")
    else:
        G_keywords = dict(zip(G_document.UaKeywords, G_document.EngKeywords))
        G_languages = ("english", "українська")
        root = ThemedTk(theme="black")
        root.resizable(False, False)
        root.config(bg="#424242")
        root.title("Перевірка написання")
        style = ttk.Style()
        style.configure("TEntry", fieldbackground="silver")
        style.configure("TLabel", font=("courier", 12))
        style.configure("TButton", font=("courier", 12))
        style.configure("W.TButton", font=("courier", 12))
        style.configure("W.TLabel", font=("courier", 14))
        style.configure("Res.TLabel", font=("courier", 14), foreground="red")
        menu_frame = ttk.Frame(root)
        lb1 = ttk.Label(menu_frame, text="Виберіть мову для написання нижче")
        cb_get = StringVar()
        cb = ttk.Combobox(menu_frame, textvariable=cb_get, values=G_languages, state="readonly", font=("courier", 12))
        cb.current(0)
        but = ttk.Button(menu_frame, text="Почати!", width=20, command=Start)
        but2 = ttk.Button(menu_frame, text="Додати слова", width=20, command=lambda: (menu_frame.pack_forget(),
                                                                                      adding_frame.pack()))
        lb1.pack(padx=50, pady=(20, 0))
        cb.pack(padx=50, pady=(0, 10))
        but.pack(padx=50, pady=10)
        but2.pack(padx=50, pady=(10, 50))
        menu_frame.pack()
        game_frame = ttk.Frame(root)
        back_but = ttk.Button(game_frame, text=" < В меню", style="W.TButton", command=lambda: (
            game_frame.pack_forget(), menu_frame.pack()))
        lab2 = ttk.Label(game_frame, text="Напишіть переклад:\n", width=50, style="W.TLabel", justify="center")
        entry1 = ttk.Entry(game_frame, font=('courier', 14, 'bold'), width=30)
        entry1.bind("<Return>", EnterPressed)
        entry1.focus()
        res_lab = ttk.Label(game_frame, width=50, style="Res.TLabel", justify="center")
        toedit_but = ttk.Button(game_frame, text="Редагувати", style="W.TButton", command=OpenEditFrame)
        back_but.pack(padx=10, pady=10, side="left")
        lab2.pack(padx=10, pady=10)
        entry1.pack(padx=10, pady=10, anchor="w")
        res_lab.pack(padx=10, pady=30)
        toedit_but.pack(padx=10, pady=10, anchor="w")
        edit_frame = ttk.Frame(root)
        togame_but = ttk.Button(edit_frame, text=" < Назад", style="W.TButton", command=lambda: (
            edit_frame.pack_forget(), game_frame.pack()))
        edit_label = ttk.Label(edit_frame, text="Редагування слів", style="W.TLabel", justify="center")
        edit_entry1 = ttk.Entry(edit_frame, font=('courier', 14, 'bold'), width=30)
        edit_entry2 = ttk.Entry(edit_frame, font=('courier', 14, 'bold'), width=30)
        edit_save_but = ttk.Button(edit_frame, text="Зберегти", width=10, style="W.TButton", command=SaveEditedWords)
        edit_clear_but = ttk.Button(edit_frame, text="Видалити", width=10, style="W.TButton", command=ClearWords)
        togame_but.grid(row=0, column=0, rowspan=3, padx=16, pady=10)
        edit_label.grid(row=0, column=1, columnspan=2, padx=10, pady=10)
        edit_entry1.grid(row=1, column=1, padx=10, pady=10)
        edit_entry2.grid(row=1, column=2, padx=10, pady=10)
        edit_save_but.grid(row=2, column=1, padx=10, pady=16, sticky="e")
        edit_clear_but.grid(row=2, column=2, padx=10, pady=16, sticky="w")
        adding_frame = ttk.Frame(root)
        but3 = ttk.Button(adding_frame, text=" < В меню", style="W.TButton", command=lambda: (
            adding_frame.pack_forget(), menu_frame.pack()))
        lab3 = ttk.Label(adding_frame, text=f"Добавити свої слова з\nперекладом {G_languages[::-1]}",
                         style="W.TLabel", justify="center")
        entry2 = ttk.Entry(adding_frame, font=('courier', 14, 'bold'), width=30)
        entry3 = ttk.Entry(adding_frame, font=('courier', 14, 'bold'), width=30)
        but4 = ttk.Button(adding_frame, text="Добавити", width=14, style="W.TButton", command=AddWords)
        but3.grid(row=0, column=0, rowspan=3, padx=16, pady=10)
        lab3.grid(row=0, column=1, columnspan=2, padx=10, pady=10)
        entry2.grid(row=1, column=1, padx=10, pady=10)
        entry3.grid(row=1, column=2, padx=10, pady=10)
        but4.grid(row=2, column=1, columnspan=2, padx=10, pady=16)
        root.mainloop()
