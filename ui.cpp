#include "ui.h"

std::vector<std::size_t> parsePath(const std::string& s,char sep)
{
    std::vector<std::size_t> out;
    std::stringstream ss(s); std::string tok;
    while(std::getline(ss,tok,sep)){
        if(tok.empty()||tok==".") continue;
        if(tok==".."){ if(out.empty()) throw MyException(ErrorType::OutOfRange,8); out.pop_back(); }
        else{
            for(char c:tok) if(!std::isdigit(c)) throw MyException(ErrorType::InvalidArg,1);
            out.push_back(std::stoull(tok));
        }
    }
    return out;
}

template<typename T>
void printTree(const NAryTree<T>& tr)
{
    using N = typename NAryTree<T>::Node;
    if (!tr.root()) { std::cout << "<empty>\n"; return; }

    const std::size_t h     = tr.height();      // высота дерева
    const std::size_t arity = tr.degree();      // ≤ 3

    std::vector<N*> curr { tr.root() };         // узлы текущего уровня

    for (std::size_t lvl = 0; lvl < h; ++lvl)
    {
        std::size_t indent  = (1ULL << (h - lvl)) - 1;          // 2^(h-lvl) - 1
        std::size_t gap     = (1ULL << (h - lvl + 1)) - 1;      // между узлами

        /* ---------- печать уровня ---------- */
        std::cout << std::string(indent, ' ');
        for (std::size_t i = 0; i < curr.size(); ++i)
        {
            if (curr[i])
                std::cout << curr[i]->value;
            else
                std::cout << ' ';

            if (i + 1 != curr.size())
                std::cout << std::string(gap, ' ');
        }
        std::cout << '\n';

        /* ---------- формируем следующий уровень ---------- */
        std::vector<N*> next;
        for (N* n : curr)
        {
            if (n) {
                for (std::size_t k = 0; k < arity; ++k)
                    next.push_back(n->children[k]);
            } else {
                for (std::size_t k = 0; k < arity; ++k)
                    next.push_back(nullptr);
            }
        }
        // если все nullptr — дальше печатать нечего
        bool allNull = true;
        for (N* n : next) if (n) { allNull = false; break; }
        if (allNull) break;

        curr.swap(next);
    }
}



static int askID(const std::vector<NAryTree<int>*>& v,const char* prompt)
{
    if(v.empty()) throw MyException(ErrorType::InvalidArg,10);
    std::cout<<prompt<<" [0.."<<v.size()-1<<"]: ";
    int id; std::cin>>id;
    if(!std::cin){ std::cin.clear(); std::cin.ignore(10000,'\n');
                   throw MyException(ErrorType::InvalidArg,0);}
    if(id<0||static_cast<std::size_t>(id)>=v.size())
        throw MyException(ErrorType::OutOfRange,9);
    return id;
}

static void createTree(std::vector<NAryTree<int>*>& objs)
{
    std::cout<<"Degree n=";
    int n; std::cin>>n;
    if(!std::cin||n<=0){ std::cin.clear(); std::cin.ignore(10000,'\n');
                         throw MyException(ErrorType::NegativeSize,2);}
    objs.push_back(new NAryTree<int>(static_cast<std::size_t>(n)));
    std::cout<<"Created tree #"<<objs.size()-1<<" (degree="<<n<<")\n";
}

static void appendEl(std::vector<NAryTree<int>*>& objs)
{
    int id = askID(objs, "Tree id");

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Path (empty for root, e.g. 0/1): ";
    std::string pathStr;
    std::getline(std::cin, pathStr);

    std::cout << "Value = ";
    int val;
    if (!(std::cin >> val)) {
        std::cin.clear(); std::cin.ignore(10000,'\n');
        throw MyException(ErrorType::InvalidArg, 1);
    }

    auto path = parsePath(pathStr);
    objs[id]->insert(path, val);
    std::cout << "Inserted.\n";
}

static void removeEl(std::vector<NAryTree<int>*>& objs)
{
    int id=askID(objs,"Tree id");
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Path (empty for root, e.g. 0/1): ";
    std::string pathStr;
    std::getline(std::cin, pathStr);

    auto path = parsePath(pathStr);
    objs[id]->smartErase(path);
    std::cout<<"Deleted.\n";
}

static void printAll(const std::vector<NAryTree<int>*>& objs)
{
    if(objs.empty()){ std::cout<<"<no trees>\n"; return; }
    for(std::size_t i=0; i<objs.size(); ++i){
        std::cout<<"--- Tree #"<<i<<" (deg="<<objs[i]->degree()
        <<", h=" << objs[i]->height() <<") ---\n";
        printTree(*objs[i]);
    }
}

static void removeTree(std::vector<NAryTree<int>*>& objs)
{
    int id=askID(objs,"Tree id to delete");
    delete objs[id];
    objs.erase(objs.begin()+id);
    std::cout<<"Tree removed.\n";
}

static void subTree(std::vector<NAryTree<int>*>& objs)
{
    if(objs.size()<2) throw MyException(ErrorType::InvalidArg,10);
    int big=askID(objs,"Main tree id");
    int small=askID(objs,"Pattern tree id");
    bool ok=objs[big]->containsSubtree(objs[small]->root());
    std::cout<<(ok?"Pattern found.\n":"Pattern NOT found.\n");
}


void runUI()
{
    std::vector<NAryTree<int>*> objs;
    bool run=true;
    while(run){
        try{
            std::cout<<"\n=== MENU ===\n"
                     <<"1) Create tree\n"
                     <<"2) Append element\n"
                     <<"3) Print ALL\n"
                     <<"4) Remove tree\n"
                     <<"5) Subtree search\n"
                     <<"6) Remove element\n"
                     <<"0) Exit\nChoose: ";
            int cmd; std::cin>>cmd;
            if(!std::cin){ std::cin.clear(); std::cin.ignore(10000,'\n');
                           throw MyException(ErrorType::InvalidArg,0);}
            switch(cmd){
                case 1: createTree(objs);  break;
                case 2: appendEl(objs);    break;
                case 3: printAll(objs);    break;
                case 4: removeTree(objs);  break;
                case 5: subTree(objs);     break;
                case 6: removeEl(objs);    break;
                case 0: run=false;         break;
                default: std::cout<<"Unknown command\n";
            }
        }catch(const MyException& ex){ handleException(ex); }
    }
    for(auto* t:objs) delete t;
    std::cout<<"Program finished.\n";
}
