# 🔍 NexusSearch
### Inverted Index Search Engine in C

---

## 🚀 Overview

NexusSearch is a high-performance document search engine built in C using core data structures and information retrieval techniques. It supports efficient indexing and fast query processing across multiple documents.

---

## 🧠 Key Concepts

- Inverted Indexing  
- Hash Tables + AVL Trees  
- TF-IDF Ranking  
- Positional Indexing (Phrase Search)

---

## ✨ Features

- 🔍 Fast word search  
- 📄 Multi-document indexing  
- 🧹 Stop-word removal  
- 📊 Relevance scoring (TF-IDF)  
- 🧠 Phrase search using positional matching  

---

## 📁 Project Structure

```
main.c
docs/
├── doc1.txt
├── doc2.txt
├── doc3.txt
```

---

## ⚙️ Compile & Run

```bash
gcc main.c -o nexussearch -lm
./nexussearch
```

---

## 🔎 Example Queries

```
data
"data structures"
algorithms
```

---

## 🚀 Demo

```bash
Enter query: "data structures"

Phrase results:
Doc:1 Para:1 Line:1 StartWord:1
```

---

## 💡 Why this project?

This project demonstrates how core data structures like **AVL trees** and **hash tables** can be combined to build a real-world **search engine** with efficient indexing and retrieval.

---

## 🚧 Future Scope

- Boolean Query Engine (AND/OR)  
- Ranked Top-K Results  
- GUI / Web Interface  

---

## 👤 Author

Parikshith A
