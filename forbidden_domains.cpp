#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


// Domain class represents a domain and provides methods to compare and check subdomains.
class Domain {
public:
    // Constructor allowing construction of an object from a string.
    explicit Domain(const std::string& str) : domain_str(str) {}

    // Overloaded equality operator for comparing two domains.
    bool operator==(const Domain& other) const {
        return domain_str == other.domain_str;
    }

    // Overloaded greater than operator for comparing two domains.
    bool operator>(const Domain& other) const {
        return domain_str > other.domain_str;
    }

    // Overloaded less than operator for comparing two domains.
    bool operator<(const Domain& other) const {
        return domain_str < other.domain_str;
    }

    // Method to check if the current domain is a subdomain of another domain.
    bool IsSubdomain(const Domain& other) const {
        return domain_str.length() > other.domain_str.length() &&
               domain_str.compare(0, other.domain_str.length(), other.domain_str) == 0;
    }

    // Helper method to get the string representation of the domain.
    std::string ToString() const {
        return domain_str;
    }

private:
    std::string domain_str;
};

// DomainChecker class manages a list of forbidden domains and provides methods to check if a domain is forbidden.
class DomainChecker {
public:
    // Constructor accepting a range of forbidden domains.
    DomainChecker(std::vector<Domain>::const_iterator begin, std::vector<Domain>::const_iterator end)
        : forbidden_domains_(begin, end) {
        std::sort(forbidden_domains_.begin(), forbidden_domains_.end());
        auto it = std::unique(forbidden_domains_.begin(), forbidden_domains_.end(), IsEqualDomain);
        size_t new_len = std::distance(forbidden_domains_.begin(), it);
        forbidden_domains_2.assign(forbidden_domains_.begin(), forbidden_domains_.begin() + new_len);

        // Save sorted and unique forbidden domains to a file for reference.
        std::ofstream fout("1.txt");
        for (const auto& x : forbidden_domains_2)
            fout << x.ToString() << std::endl;
    }

    // Method to check if a domain is forbidden.
    bool IsForbidden(const Domain& domain) const {
        int low = 0, high = forbidden_domains_2.size() - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (domain == forbidden_domains_2[mid] || domain.IsSubdomain(forbidden_domains_2[mid]))
                return true;
            if (domain > forbidden_domains_2[mid])
                low = mid + 1;
            else
                high = mid - 1;
        }
        return false;
    }

private:
    std::vector<Domain> forbidden_domains_;
    std::vector<Domain> forbidden_domains_2;

    // Static method to check if two domains are equal or if one is a subdomain of the other.
    static bool IsEqualDomain(const Domain& domain1, const Domain& domain2) {
        return (domain1 == domain2 || domain2.IsSubdomain(domain1));
    }
};

// Function to read domains from the input stream.
std::vector<Domain> ReadDomains(std::istream& input, size_t count) {
    std::vector<Domain> domains;
    for (size_t i = 0; i < count; ++i) {
        std::string domain_str;
        std::getline(input, domain_str);
        domain_str = "." + domain_str;
        std::reverse(domain_str.begin(), domain_str.end());
        domains.emplace_back(domain_str);
    }
    return domains;
}

// Template function to read a number from a line in the input stream.
template <typename Number>
Number ReadNumberOnLine(std::istream& input) {
    std::string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

int main() {
    // Read forbidden domains and create a DomainChecker.
    const std::vector<Domain> forbidden_domains = ReadDomains(std::cin, ReadNumberOnLine<size_t>(std::cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    // Read test domains and check if each is forbidden.
    const std::vector<Domain> test_domains = ReadDomains(std::cin, ReadNumberOnLine<size_t>(std::cin));
    for (const Domain& domain : test_domains) {
        std::cout << (checker.IsForbidden(domain) ? "Bad" : "Good") << std::endl;
    }

    return 0;
}
