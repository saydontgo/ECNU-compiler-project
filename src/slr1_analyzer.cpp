#include "slr1_analyzer.h"
int Item::count_ = 0;

std::string EMPTYCH = "E";

std::string FirstWord(std::string s)
{
    s += " ";
    std::string first = s.substr(0, s.find(" "));
    return first;
}

std::vector<std::string> Split(std::string s, std::string separator)
{
    std::vector<std::string> v;

    std::string::size_type pos1, pos2;
    pos2 = s.find(separator);
    pos1 = 0;

    while (std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + separator.size();
        pos2 = s.find(separator, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));

    return v;
}
Item::Item(std::string i) {
    id = count_++;
    left_ = i.substr(0, i.find("->"));
    right_ = i.substr(i.find("->") + 2);
    item_ = left_ + "->" + right_;

    if (right_.find(".") == std::string::npos)
        AddDot(0);
}

Item::Item(std::string l, std::string r) {
    id = count_++;
    left_ = l;
    right_ = r;
    item_ = left_ + "->" + right_;

    if (right_.find(".") == std::string::npos)
        AddDot(0);
}

auto Item::GetLeft() const -> const std::string& {
    return left_;
}

auto Item::GetRight() const -> const std::string& {
    return right_;
}

auto Item::GetItem() -> const std::string& {
    item_ = left_ + "->" + right_;
    return item_;
}

auto Item::GetDot() const -> int {
    return item_.find(".");
}

void Item::AddDot(int pos) {
    if (right_[pos] == '#')
            right_ = ".";
    else if (pos == 0)
        right_.insert(pos, ". ");
    else if (pos == right_.size())
        right_.insert(pos, " .");
    else
        right_.insert(pos, " . ");
}

auto Item::HasNextDot() const -> bool {
    auto buffer = Split(right_, ".");
    if (buffer.size() > 1)
    {
        return true;
    }
    else
        return false;
}

auto Item::GetNext() const -> std::string {
    auto buffer = Split(item_, ".");
    buffer[1].erase(0, 1);
    return FirstWord(buffer[1]);
}

auto Item::NextDot() -> std::string {
    int dotPos = right_.find(".");
    auto buffer = Split(item_, ".");
    buffer[1].erase(0, 1);
    auto first = FirstWord(buffer[1]);
    int nextPos = dotPos + first.size();
    right_.erase(right_.find("."), 2);
    right_.insert(nextPos, " .");
    return right_;
}

auto Item::operator==(Item& other) -> bool {
    return GetItem() == other.GetItem();
}