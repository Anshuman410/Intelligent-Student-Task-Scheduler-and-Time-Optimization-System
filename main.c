#include <windows.h>
#include "auth.h"
#include "dashboard.h"

int main() {
    if (run_auth_gui()) {   // login successful
        run_dashboard();    // open dashboard GUI
    }
    return 0;
}

