'use strict';

// Color priority:
// gray             - not supported / not available
// blue             - not good as green but still OK
// green            - OK
// yellow           - worse
// orange           - more worse
// red              - the worst condition

const CPU_Thermal = {
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

exports.CPU_Thermal = CPU_Thermal;

let CPU_Thermal_Ranges = [
    { status: CPU_Thermal.Cold, t_max: 0 },
    { status: CPU_Thermal.Normal, t_max: 40 },
    { status: CPU_Thermal.Warm, t_max: 50 },
    { status: CPU_Thermal.Hot, t_max: 70 },
    { status: CPU_Thermal.Critical, t_max: 1000 },
];

exports.CPU_Thermal_Ranges = CPU_Thermal_Ranges;

const CPU_Thermal_RangesInit = (() => {
    for (let i = 1; i < CPU_Thermal_Ranges.length; ++i) {
        if (i == 1)
            CPU_Thermal_Ranges[i - 1].t_min = -1000;

        CPU_Thermal_Ranges[i].t_min = CPU_Thermal_Ranges[i - 1].t_max;
    }
})();

exports.GNSS = {
    None: { text: '', color: 'gray' },
    NotSupported: { text: 'not supported', color: 'gray' },
    NoData: { text: 'no data', color: 'orange' },
    FileNameError: { text: 'file name format error', color: 'orange' },
    FormatError: { text: 'data format error', color: 'orange' },
    TimeError: { text: 'time error', color: 'orange' }, // e.g. GPS spoofing
    Invalid: { text: 'not valid', color: 'yellow' },
    Valid: { text: '', color: 'green' },
}
