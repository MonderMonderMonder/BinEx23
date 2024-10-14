#include <iostream>
#include <iomanip>
#include <map>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>

#define min(a,b) (((a)<(b))?a:b)
#define max(a,b) (((a)<(b))?b:a)

class String {
private:
    char *data;
    size_t len;    
public:
    String() : len(0) {data = new char[1];}
    String(const char *str) {
        len = strlen(str) + 1;
        data = new char [len];
        strcpy(data, str);
    }

    String& operator=(const char *str) {
        if (strlen(str) > len) {
            delete[] data;
            data = new char[strlen(str) + 1];
        }
        strcpy(data, str);
        return *this;
    }

    String operator+(String &other) {
        String ret;
        // other.len + 1 is save as the buffer is at least of size len + 1
        // and therefore len can not be itself SIZE_MAX, so that len + 1 will not wrap
        if (SIZE_MAX - (len + 1) <= other.len)
            throw std::length_error("overflow calculating the new String array length!");
        ret.data = new char[len + other.len + 1];
        if (ret.data == nullptr)
            throw std::runtime_error("not enough memory");
        ret.len = this->len + other.len;
        strncat(ret.data, this->data, ret.len + 1);
        strncat(ret.data, other.data, ret.len + 1);
        return ret;
    }

    friend std::ostream& operator<< (std::ostream& out, const String str) {
        out << str.data;
        return out;
    }

    virtual ~String() {
        delete[] data;
    }
};

std::map<unsigned long, String> strings;

bool parse(char *statement) {
    enum {ST_NONE, ST_SELECT, ST_SFIELDS, ST_SFROM, ST_SSTRINGS, ST_SWHEREAND, ST_SID, ST_SCMP, ST_SBND, 
        ST_INSERT, ST_IINTO, ST_ISTRINGS, ST_IVALUES, ST_IOPEN, ST_IID, ST_ISTR, ST_ICLOSE, ST_ICOMMA,
        ST_UPDATE, ST_USTRINGS, ST_USET, ST_USTR, ST_UEQ, ST_UNSTR, ST_UWHERE, ST_UID, ST_UWEQ, ST_UCOND,
        ST_SEMICOL} state = ST_NONE;
    enum {Q_SELECT, Q_INSERT, Q_UPDATE} type;
    enum {CMP_LE, CMP_L, CMP_GE, CMP_G, CMP_EQ} cmp;
    char *nxt;
    char *cur = strtok_r(statement, " \t", &nxt);
    const char *spliton = " \t\n";
    unsigned long lb =  0;
    unsigned long ub = SIZE_MAX;
    unsigned long id, bnd;
    const char *str;
    bool field_id = true;
    bool field_str = true;
    while (state != ST_SEMICOL) {
        if (cur == nullptr)
            return false;
        spliton = " \t\n";
        switch (state) {
        case ST_NONE:
            if (strcasecmp(cur, "select") == 0) {
                state = ST_SELECT;
                type = Q_SELECT;
            } else if (strcasecmp(cur, "insert") == 0) {
                state = ST_INSERT;
                type = Q_INSERT;
            } else {
                state = ST_UPDATE;
                type = Q_UPDATE;
            }
            break;
        // select statement
        case ST_SELECT:
            if (strcmp(cur, "id") == 0)
                field_str = false;
            else if (strcmp(cur, "str") == 0)
                field_id = false;
            state = ST_SFIELDS;
            break;
        case ST_SFIELDS:
            state = ST_SFROM;
            break;
        case ST_SFROM:
            if (cur[strlen(cur) - 1] == ';')
                state = ST_SEMICOL;
            else
                state = ST_SSTRINGS;
            break;
        case ST_SSTRINGS:
            if (strcmp(cur, ";") == 0)
                state = ST_SEMICOL;
            else
                state = ST_SWHEREAND;
            break;
        case ST_SWHEREAND:
            state = ST_SID;
            if (strlen(cur) <= 2)
                break;
            cur += 2;
            // fallthrough             
        case ST_SID:
            state = ST_SCMP;
            if (cur[0] == '<') {
                if (cur[1] == '=')
                    cmp = CMP_LE;
                else
                    cmp = CMP_L;
            } else if (cur[0] == '>') {
                if (cur[1] == '=')
                    cmp = CMP_GE;
                else
                    cmp = CMP_G;
            } else
                cmp = CMP_EQ;
            if (strlen(cur) > ((cmp == CMP_LE || cmp == CMP_GE)?2:1))
                cur += (cmp == CMP_LE || cmp == CMP_GE)?2:1;
            else
                break;
            // fallthrough             
        case ST_SCMP:
            state = ST_SBND;
            char *aft;
            bnd = strtoul(cur, &aft, 10);
            switch (cmp) {
            case CMP_L:
                bnd -= 1;
                // fallthrough                
            case CMP_LE:
                ub = min(ub, bnd);
                break;
            case CMP_G:
                bnd += 1;
                // fallthrough                
            case CMP_GE:
                lb = max(lb, bnd);
                break;
            default:
                lb = max(lb, bnd);
                ub = min(ub, bnd);
            }
            if (*aft == ';')
                state = ST_SEMICOL;
            break;
        case ST_SBND:
            if (strcmp(cur, ";") == 0)
                state = ST_SEMICOL;
            else
                state = ST_SWHEREAND;
            break;
        
        // insert statement
        case ST_INSERT:
            state = ST_IINTO;
            break;
        case ST_IINTO:
            state = ST_ISTRINGS;
            break;
        case ST_ISTRINGS:
            state = ST_IVALUES;
            break;
        case ST_IVALUES:
            state = ST_IOPEN;
            if (cur[1] == '\0')
                break;
            ++cur;
            // fallthrough             
        case ST_IOPEN:
            state = ST_IID;
            id = strtoul(cur, &aft, 0);
            if (*aft == '\0')
                break;
            else cur = aft;
            // fallthrough             
        case ST_IID:
            if (nxt[0] == '"' && nxt[1] == '"') {
                str = "";
                state = ST_ISTR;
                nxt += 2;
            } else {
                spliton = "\"";
                state = ST_ICOMMA;
            }
            break;
        case ST_ICOMMA:
            str = cur;
            state = ST_ISTR;
            break;
        case ST_ISTR:
            if (cur[1] == ';')
                state = ST_SEMICOL;
            else
                state = ST_ICLOSE;
            break;
        case ST_ICLOSE:
            state = ST_SEMICOL;
            break;

        // update
        case ST_UPDATE:
            state = ST_USTRINGS;
            break;
        case ST_USTRINGS:
            state = ST_USET;
            break;
        case ST_USET:
            state = ST_USTR;
            if (strlen(cur) > 3)
                cur += 3;
            else
                break;
            // fallthrough             
        case ST_USTR:
            if (nxt[0] == '"' && nxt[1] == '"') {
                str = "";
                state = ST_UNSTR;
                nxt += 2;
            } else {
                spliton = "\"";
                state = ST_UEQ;
            }
            break;
        case ST_UEQ:
            state = ST_UNSTR;
            str = cur;
            break;
        case ST_UNSTR:
            state = ST_UWHERE;
            break;
        case ST_UWHERE:
            state = ST_UID;
            if (strlen(cur) > 2)
                cur += 2;
            else break;
            // fallthrough             
        case ST_UID:
            state = ST_UWEQ;
            if (strlen(cur) > 1)
                cur += 1;
            else break;
            // fallthrough             
        case ST_UWEQ:
            state = ST_UCOND;
            id = strtoul(cur, &aft, 10);
            if (*aft == ';')
                state = ST_SEMICOL;
            break;
        case ST_UCOND:
            state = ST_SEMICOL;
            break;
        case ST_SEMICOL:
            break;
        }

        cur = strtok_r(nullptr, spliton, &nxt);
    }

    std::cout << std::left;
    switch (type) {
    case Q_SELECT:
        if (field_id)
            std::cout << std::setw(10) << "id";
        if (field_id && field_str) 
            std::cout << " | ";
        if (field_str)
            std::cout << std::setw(90) << "str";
        std::cout << std::endl;
        std::cout << "-------------------------------------------------------------------------------------------------------" << std::endl;
        
        if (ub == lb) {
            if (field_id)
                std::cout << std::setw(10) << ub;
            if (field_id && field_str) 
                std::cout << " | ";
            if (field_str)
                std::cout << std::setw(90) << strings[ub];
            std::cout << std::endl;
            break;
        }
        
        if (ub < lb) break;

        for (std::map<unsigned long, String>::iterator it = strings.lower_bound(lb); it != strings.upper_bound(ub); it++) {
            if (field_id)   
                std::cout << std::setw(10) << it->first;
            if (field_id && field_str)
                std::cout << " | ";
            if (field_str)
                std::cout << std::setw(90) << it->second;
            std::cout << std::endl;
        }
        break;
    case Q_INSERT:
        if (strings.find(0) == strings.end()) {
            std::cout << "insufficient privileges" << std::endl;
            break;
        }
        if (strings.find(id) != strings.end()) {
            std::cout << "element already exists!" << std::endl;
            break;
        }
        strings.emplace(id, str);
        break;
    case Q_UPDATE:
        if (strings.find(0) == strings.end()) {
            std::cout << "insufficient privileges" << std::endl;
            break;
        }
        if (strings.find(id) != strings.end())
            strings[id] = str;
        break;
    }

    return true;
}

void banner() {
    std::cout << "Welcome to myFirst\033[0;33mSQL\033[0mExperience(TM)" << std::endl;
}

#define UNUSED __attribute__((unused))

int main (int UNUSED argc, const char UNUSED *argv[]) {
    char *line = nullptr;
    size_t len = 0;
    banner();
    while(getline(&line, &len, stdin) > 0) {
        parse(line);
    }
    return EXIT_SUCCESS;
}
