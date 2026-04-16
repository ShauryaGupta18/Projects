#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

// ---------------------- STOCK CLASS ----------------------
class Stock {
private:
    string symbol;
    int quantity;
    double purchasePrice;
    double currentPrice;

public:
    Stock(string sym, int qty, double pPrice, double cPrice) {
        symbol = sym;
        quantity = qty;
        purchasePrice = pPrice;
        currentPrice = cPrice;
    }

    string getSymbol() const { return symbol; }
    int getQuantity() const { return quantity; }

    double investedValue() const {
        return quantity * purchasePrice;
    }

    double currentValue() const {
        return quantity * currentPrice;
    }

    double profitLoss() const {
        return currentValue() - investedValue();
    }

    double returnPercent() const {
        return (profitLoss() / investedValue()) * 100;
    }

    double getPurchasePrice() const { return purchasePrice; }

    void display() const {
        cout << left << setw(8) << symbol
             << setw(10) << quantity
             << setw(12) << purchasePrice
             << setw(12) << currentPrice
             << setw(12) << currentValue()
             << setw(12) << profitLoss()
             << setw(10) << returnPercent()
             << endl;
    }
};

// ---------------------- PORTFOLIO CLASS ----------------------
class Portfolio {
private:
    vector<Stock> holdings;

public:
    void addStock(Stock s) {
        holdings.push_back(s);
    }

    double totalInvested() const {
        double total = 0;
        for (int i = 0; i < holdings.size(); i++) {
            total += holdings[i].investedValue();
        }
        return total;
    }

    double totalCurrentValue() const {
        double total = 0;
        for (int i = 0; i < holdings.size(); i++) {
            total += holdings[i].currentValue();
        }
        return total;
    }

    double totalProfitLoss() const {
        return totalCurrentValue() - totalInvested();
    }

    double portfolioReturn() const {
        return (totalProfitLoss() / totalInvested()) * 100;
    }

    // Weighted average buy price
    double averageBuyPrice() const {
        double totalCost = 0;
        int totalQty = 0;

        for (int i = 0; i < holdings.size(); i++) {
            totalCost += holdings[i].investedValue();
            totalQty += holdings[i].getQuantity();
        }

        return totalCost / totalQty;
    }

    // Best performing stock
    Stock bestPerformer() const {
        Stock best = holdings[0];
        for (int i = 1; i < holdings.size(); i++) {
            if (holdings[i].returnPercent() > best.returnPercent()) {
                best = holdings[i];
            }
        }
        return best;
    }

    // Worst performing stock
    Stock worstPerformer() const {
        Stock worst = holdings[0];
        for (int i = 1; i < holdings.size(); i++) {
            if (holdings[i].returnPercent() < worst.returnPercent()) {
                worst = holdings[i];
            }
        }
        return worst;
    }

    void showDiversification() const {
        double total = totalCurrentValue();

        cout << "\n--- Diversification ---\n";
        for (int i = 0; i < holdings.size(); i++) {
            double percent = (holdings[i].currentValue() / total) * 100;
            cout << holdings[i].getSymbol()
                 << ": " << fixed << setprecision(2)
                 << percent << "%\n";
        }
    }

    void display() const {
        cout << "\n=========== Portfolio Report ===========\n";

        cout << left << setw(8) << "Symbol"
             << setw(10) << "Qty"
             << setw(12) << "Buy"
             << setw(12) << "Current"
             << setw(12) << "Value"
             << setw(12) << "P/L"
             << setw(10) << "Return%"
             << endl;

        cout << "---------------------------------------------------------------\n";

        for (int i = 0; i < holdings.size(); i++) {
            holdings[i].display();
        }

        cout << "---------------------------------------------------------------\n";

        cout << "Total Invested : $" << totalInvested() << endl;
        cout << "Current Value  : $" << totalCurrentValue() << endl;
        cout << "Total P/L      : $" << totalProfitLoss() << endl;
        cout << "Portfolio Return: " << portfolioReturn() << "%\n";
        cout << "Avg Buy Price  : $" << averageBuyPrice() << endl;

        Stock best = bestPerformer();
        Stock worst = worstPerformer();

        cout << "\nBest Performer : " << best.getSymbol() << endl;
        cout << "Worst Performer: " << worst.getSymbol() << endl;

        showDiversification();
    }
};

// ---------------------- MAIN ----------------------
int main() {

    Portfolio p;

    // Predefined data (no user input)
    p.addStock(Stock("AAPL", 10, 150, 180));
    p.addStock(Stock("GOOGL", 5, 280, 300));
    p.addStock(Stock("TSLA", 8, 700, 650));
    p.addStock(Stock("MSFT", 6, 250, 310));

    p.display();

    return 0;
}