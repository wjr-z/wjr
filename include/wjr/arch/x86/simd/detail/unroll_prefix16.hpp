const size_t rem16 = (WJRX_COUNT - 5) % 16 + 5;
switch (rem16) {
case 5: {
    WJRX_SCALAR(4);
    break;
}
case 6: {
    WJRX_UNROLL2(4);
    break;
}
case 9:
case 10:
case 11:
case 12: {
    WJRX_UNROLL4(4);
    goto __rem4;
}
case 13:
case 14:
case 15:
case 16: {
    WJRX_UNROLL4(4);
    WJRX_UNROLL4(8);
    goto __rem4;
}
case 17:
case 18:
case 19:
case 20: {
    WJRX_UNROLL4(4);
    WJRX_UNROLL4(8);
    WJRX_UNROLL4(12);
    WJR_FALLTHROUGH;
}
case 7:
case 8: {
__rem4:
    WJRX_UNROLL4(rem16 - 4);
    break;
}
default: {
    WJR_UNREACHABLE();
}
}

// This maybe always false
if (rem16 == WJRX_COUNT) {
    return WJRX_RETURN(WJRX_COUNT);
}