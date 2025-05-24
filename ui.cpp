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
static std::size_t maxWidth(const typename NAryTree<T>::Node* n)
{
    if (!n) return 1;
    std::size_t w = std::to_string(n->value).size();
    for (auto* c : n->children) {
        w = std::max<std::size_t>(w, maxWidth<T>(c));
    }
    return w + 1;
}

template<typename T>
void printTree(const NAryTree<T>& tr)
{
    using N = typename NAryTree<T>::Node;
    if (!tr.root()) {
        std::cout << "<empty>\n"; 
        return;
    }

    const std::size_t h = tr.height();
    const std::size_t n = tr.degree();

    const std::size_t lineW = maxWidth<T>(tr.root()) * (std::pow(n, h-1));

    std::vector<N*> layer { tr.root() };

    for (std::size_t lvl = 0; lvl < h; ++lvl)
    {
        std::string line(lineW, ' ');

        const std::size_t slots = std::pow(n, lvl);

        for (std::size_t k = 0; k < slots; ++k) {
            std::size_t idx = k < layer.size() ? k : 0;
            N* node = (idx < layer.size() ? layer[idx] : nullptr);

            std::string val = node ? std::to_string(node->value) : "X";

            std::size_t center = ( (2*k + 1) * lineW ) / (2*slots);

            std::size_t start = center - val.size()/2;
            line.replace(start, val.size(), val);
        }
        std::cout << line << '\n';

        std::vector<N*> next;
        for (N* nd : layer) {
            for (std::size_t k = 0; k < n; ++k) {
                next.push_back(nd ? nd->children[k] : nullptr);
            }
        }
        bool any = false;
        for (N* x : next) if (x) { any = true; break; }
        if (!any) break;

        layer.swap(next);
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


static int rndInt(int lo,int hi)
{
    return rand() % (hi - lo + 1) + lo;
}

template<typename T>
static void shuffle(std::vector<T>& v)
{
    for (std::size_t i = v.size(); i > 1; --i) {
        std::size_t j = rand() % i;
        std::swap(v[i-1], v[j]);
    }
}

void createRandomTree(std::vector<NAryTree<int>*>& objs)
{
    srand(static_cast<unsigned>(time(nullptr)));

    createTree(objs);
    auto* tree = objs.back();

    int H, pct, lo, hi;
    std::cout << "Target height (>=1): "; std::cin >> H;
    std::cout << "Fill percent  [0-100]: "; std::cin >> pct;
    std::cout << "Value range   [lo hi]: "; std::cin >> lo >> hi;
    if(!std::cin || H<=0 || pct<0 || pct>100 || lo>hi) {
        throw MyException(ErrorType::InvalidArg,1);
    }
        
    tree->insert({}, rndInt(lo,hi));

    const std::size_t n = tree->degree();
    std::vector<std::vector<std::size_t>> parents = {{}};

    for (int lvl=1; lvl<H && !parents.empty(); ++lvl)
    {
        std::vector<std::vector<std::size_t>> slots;
        for (const auto& p : parents) {
            for (std::size_t k=0;k<n;++k){
                auto q=p; q.push_back(k);
                slots.emplace_back(std::move(q));
            }
        }
        std::size_t need = (slots.size()*pct + 50)/100;

        shuffle(slots);
        std::vector<std::vector<std::size_t>> nextParents;

        for (std::size_t i=0; i<need; ++i) {
            tree->insert(slots[i], rndInt(lo,hi));
            nextParents.push_back(std::move(slots[i]));
        }
        parents.swap(nextParents);
    }

    std::cout << "Random tree created (height="
              << tree->height() << ", each level ≈ "
              << pct << "% full)\n";
}


static std::string pathToStr(const std::vector<std::size_t>& p)
{
    if (p.empty()) return "(root)";
    std::string s;
    for (std::size_t i = 0; i < p.size(); ++i) {
        if (i) s.push_back('/');
        s += std::to_string(p[i]);
    }
    return s;
}

void findEl(std::vector<NAryTree<int>*>& objs) {
    if (objs.empty()) {
        throw MyException(ErrorType::InvalidArg, 10);
    }
    int id = askID(objs, "Tree id");
    NAryTree<int>* tree = objs[id];

    std::cout << "Value to search: ";
    int target; std::cin >> target;
    if (!std::cin) { 
        std::cin.clear();
        std::cin.ignore(10000,'\n');
        throw MyException(ErrorType::InvalidArg, 1);
    }

    std::cout << "Level h (1 = root): ";
    int h;
    std::cin >> h;
    h--;
    if (!std::cin || h < 0) {
        std::cin.clear();
        std::cin.ignore(10000,'\n');
        throw MyException(ErrorType::InvalidArg, 1);
        }

    using Pair = std::pair<typename NAryTree<int>::Node*, std::vector<std::size_t>>;
    std::queue<Pair> q;
    q.push({tree->root(), {}});

    std::vector<std::vector<std::size_t>> found;
    int level = 0;

    while (!q.empty() && level <= h)
    {
        std::size_t sz = q.size();
        while (sz--) {
            auto [node, path] = q.front(); q.pop();
            if (level == h && node && node->value == target)
                found.push_back(path);

            if (level < h && node) {
                for (std::size_t k = 0; k < tree->degree(); ++k) {
                    auto p = path; p.push_back(k);
                    q.push({node->children[k], std::move(p)});
                }
            }
        }
        level++;
    }

    if (found.empty()) {
        std::cout << "No nodes == " << target << " at level " << h+1 << '\n';
    } else {
        std::cout << "Paths:\n";
        for (std::size_t i = 0; i < found.size(); ++i) {
            if (i) std::cout << "\n";
            std::cout << pathToStr(found[i]);
        }
        std::cout << '\n';
    }
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
                     <<"7) Create random tree\n"
                     <<"8) Find element\n"
                     <<"0) Exit\nChoose: ";
            int cmd; std::cin>>cmd;
            if(!std::cin){ std::cin.clear(); std::cin.ignore(10000,'\n');
                           throw MyException(ErrorType::InvalidArg,0);}
            switch(cmd){
                case 1: createTree(objs);       break;
                case 2: appendEl(objs);         break;
                case 3: printAll(objs);         break;
                case 4: removeTree(objs);       break;
                case 5: subTree(objs);          break;
                case 6: removeEl(objs);         break;
                case 7: createRandomTree(objs); break;
                case 8: findEl(objs);           break;
                case 0: run=false;              break;
                default: std::cout<<"Unknown command\n";
            }
        }catch(const MyException& ex){ handleException(ex); }
    }
    for(auto* t:objs) delete t;
    std::cout<<"Program finished.\n";
}
