'use strict';

const sc_status = require('./status.js');
const sc_utils = require('./utils.js');

exports.GetPage = function () {
    return `<tr><td></td><td>- load avg.:</td><td id="cpu_loadavg"></td></tr>
<tr><td id="cpu_thermal_color" bgcolor="white"></td><td>CPU thermal:</td><td id="cpu_thermal_text"></td></tr>`;
}

exports.GetPageData = function (a_config) {
    let data = {};
    data.cpu_loadavg = sc_utils.GetLoadAvg();
    data.cpu_thermal = GetThermal('/sys/class/hwmon/hwmon0/temp1_input', a_config.cpu_thermaldiv);
    return data;
}

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
