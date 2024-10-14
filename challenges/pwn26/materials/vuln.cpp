#include <iostream>
#include <vector>
#include <limits>
#include <unistd.h>
#include <cstring>

class string
{
    public:
        string(unsigned n = 0)
        {
            this->len = n;
            this->data = new char[this->len];
            memset(this->data, 0, this->len);
        }
        string(char const *s)
        {
            new(this) string(strlen(s));
            memcpy(this->data, s, this->len);
        }
        virtual ~string()
        {
            delete[] this->data;
        }
        friend std::ostream& operator<<(std::ostream& o, const string &s)
        {
            o.write(s.data, s.len);
            return o;
        }
        size_t length() { return this->len; }
        void get(std::istream &i, unsigned l)
        {
            i.getline(this->data, l, '\n');
        }
        void exec();
    protected:
        char *data = nullptr;
        size_t len = 0;
};
void string::exec()
{
    if (this->len == strlen(this->data) + 1)
        system(this->data);
}

/* much safe. such string. wow. */
class safe_string: public string
{
    public:
        safe_string(unsigned n = 0): string(n) {}
        safe_string(char const *s): string(s) {}
        virtual ~safe_string()
        {
            for (size_t i = 0; i < this->len; ++i)
                this->data[i] = "fuckaduck"[i % 9];
        }
        void exec() { /* safe string is safe. */ }
};

constexpr auto infty = std::numeric_limits<std::streamsize>::max();
int main()
{
    std::vector<string *> strs;
    std::string cmd;

    std::cin >> std::skipws;

    strs.push_back(new safe_string("static prog"));

    while (true) {

        if (!(std::cin >> cmd).ignore(infty, '\n'))
            goto fail;

        if (cmd == "quit") {
            break;
        }
        else if (cmd == "show") {
            for (auto s: strs)
                std::cout << *s << std::endl;
        }
        else if (cmd == "add") {
            int n;
            if (!(std::cin >> n).ignore(infty, '\n') || (n = abs(n)) > 0x100)
                goto fail;
            strs.push_back(new safe_string(0x100));
            strs.back()->get(std::cin, n);
            if (!std::cin)
                goto fail;
        }
        else if (cmd == "edit") {
            size_t n;
            if (!(std::cin >> n).ignore(infty, '\n') || n >= strs.size())
                goto fail;
            strs[n]->get(std::cin, strs[n]->length());
        }
        else if (cmd == "delete") {
            size_t n;
            if (!(std::cin >> n).ignore(infty, '\n') || n >= strs.size())
                goto fail;
            delete strs[n];
            strs.erase(strs.begin() + n);
        }
        else {
fail:       std::cout << "\x1b[31mlolwut?\x1b[0m" << std::endl;
            break;
        }
        std::cout << "\x1b[32mok!\x1b[0m" << std::endl;

    }
}

