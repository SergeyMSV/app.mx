'use strict';

// Color priority:
// gray             - not supported / not available
// blue             - not good as green but still OK
// green            - OK
// yellow           - worse
// orange           - more worse
// red              - the worst condition

exports.CPU_Thermal = {
    None: { text: '', color: 'gray' },
    NotSupported: { text: 'not supported', color: 'gray' },
    NoData: { text: 'no data', color: 'gray' },
    Error: { text: 'error', color: 'gray' },
    Cold: { text: '', color: 'blue' },
    Normal: { text: '', color: 'green' },
    Warm: { text: '', color: 'yellow' },
    Hot: { text: '', color: 'orange' },
    Critical: { text: '', color: 'red' },
}
