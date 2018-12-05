UNREFERENCED_PARAMETER();

SetWindowLongPtr(hwnd, DWLP_USER, lParam);
GetWindowLongPtr(hwnd, DWLP_USER);

MAKELRESULT(low, high)
MAKEWPARAM(low, high)
MAKELPARAM(low, high)

GET_X_LPARAM(lp)
GET_Y_LPARAM(lp)
