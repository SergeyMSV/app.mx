'use strict';

//const sc_status = require('./status.js');
const sc_utils = require('./utils.js');

exports.GetPage = function () {
    let str = '';
    for (const i of g_hwmon)
        str += `<tr><td id="color${i.id ?? ''}"></td><td>${i.name ?? ''}</td><td id="input${i.id ?? ''}"></td></tr>`; // '...-input' contains Temperature and Humidity at the same time.
    return str;
}

exports.GetPageData = function () {
    // data - input - hwmon0 = "T: 21.35°C H: 65.43%"
    //      |       - hwmon1 = "T:121.35°C H:100.00%"
    //      |       - hwmon2 = "T:-21.35°C H:100.00%"
    //      |       - hwmon3 = "T: -2.35°C H:  2.34%"
    //      |       - ...
    //      |
    //      - color - hwmon0 = "green"
    //              - hwmon1 = "yellow"
    //              - ...
    let data = { input: {}, color: {} };
    for (let i of g_hwmon) {
        if (Object.hasOwn(i, 'temp1_input')) {
            data['input'][i.id] = GetTemperature(i.dir + '/' + 'temp1_input', 1000); // [#] 1000
        }
        if (Object.hasOwn(i, 'humidity1_input')) {
            data['input'][i.id] += ' ' + GetHumidity(i.dir + '/' + 'humidity1_input', 1000); // [#] 1000
        }
        data['color'][i.id] = 'green'; // [TBD] it depends on both things: temperature and humidity
    }
    return data;
}

let g_hwmon = (() => {
    try {
        let data = [];
        const dir = '/sys/class/hwmon';
        const dirsHwmon = sc_utils.ReadDir(dir);
        let dataIndex = 0;
        for (const dh of dirsHwmon) {
            if (dh.includes('hwmon', 0)) {
                data[dataIndex] = {};
                data[dataIndex].id = dh;
                const dirHwmonX = dir + '/' + dh;
                data[dataIndex].dir = dirHwmonX;
                const filesHwmonX = sc_utils.ReadDir(dirHwmonX + '/'); // It's a link. For "ls -la" last slash is needed in order to show files in the directory.
                for (const fh of filesHwmonX) {
                    if (fh == 'name' || fh == 'label' || fh == 'temp1_input' || fh == 'humidity1_input')
                        data[dataIndex][fh] = sc_utils.ReadFile(dirHwmonX + '/' + fh);
                    else if (fh == 'of_node') {
                        const regStr = sc_utils.ReadFile(dirHwmonX + '/' + fh + '/reg');
                        data[dataIndex].reg = regStr ? regStr.charCodeAt(0) : 0;
                    }
                }
                ++dataIndex;
            }
        }
        return data;
    }
    catch (err) {
        console.error(err); // It can be written only once when the device is started.
    }
    return [];
})();

function GetTemperature(a_path, a_div) {
    try {
        const str = sc_utils.ReadFile(a_path, '');
        if (isNaN(parseInt(str)))
            return 'n/a';
        let val = (parseInt(str) / a_div).toFixed(2);
        val = val.padStart(6, ' '); 
        return 'T:' + val + '\xB0C'; // 0xB0 - Celsius degree sign.
    }
    catch {
        return 'err';
    }
}

function GetHumidity(a_path, a_div) {
    try {
        const str = sc_utils.ReadFile(a_path, '');
        if (isNaN(parseInt(str)))
            return 'n/a';
        let val = (parseInt(str) / a_div).toFixed(2);
        val = val.padStart(6, ' ');
        return 'H:' + val + '%';
    }
    catch {
        return 'err';
    }
}

/*
function GetThermal(a_filePath, a_thermalDiv) { // DEPRECATED
    try {
        const tempStr = sc_utils.ReadFile(a_filePath, '');
        if (isNaN(parseInt(tempStr)))
            return sc_status.CPU_Thermal.NoData;
        const temp = (parseInt(tempStr) / a_thermalDiv).toFixed(2);
        return GetThermalStatus(temp);
    }
    catch {
        return sc_status.CPU_Thermal.NotSupported;
    }
}

let g_cpu_thermal_status = sc_status.CPU_Thermal.None
function GetThermalStatus(a_value) {

    function CheckTemp(val, min, max) {
        const hysteresis = g_cpu_thermal_status != sc_status.CPU_Thermal.None ? 1 : 0;
        return val >= min + hysteresis && val <= max - hysteresis; // hysteresis
    }

    function TryStatus(val, min, max, status) {
        if (!CheckTemp(val, min, max))
            return false;
        g_cpu_thermal_status = status;
        return true;
    }

    for (const i of sc_status.CPU_Thermal_Ranges) {
        if (TryStatus(a_value, i.t_min, i.t_max, i.status))
            break;
    }

    let st = Object.assign({}, g_cpu_thermal_status);
    st.text = a_value + ' \xB0C'; // 0xB0 - celsius degree sign
    return st;
}
*/