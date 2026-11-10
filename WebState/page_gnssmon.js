'use strict';

const sc_config = require('./config.js');
const sc_utils = require('./utils.js')
const sc_utils_gnss = require('./utils_gnss.js')

exports.GetPage = function () {
    return MakePart(null) + MakePart("1") + MakePart("2");
}

exports.GetPageData = function () {
    let gnssmon = {};
    let data = MakePageData(null);
    if (data !== null)
        gnssmon.gnss = data;
    data = MakePageData("1");
    if (data !== null)
        gnssmon.gnss1 = data;
    data = MakePageData("2");
    if (data !== null)
        gnssmon.gnss2 = data;
    return gnssmon;
}

function MakePart(a_id) {
    const data = MakePageData(a_id);
    if (data === null)
        return "";
    const idStr = "gnss" + (a_id ?? "");
    const str = `<tr><td id="color${idStr}" bgcolor="white"></td><td>GNSS ${a_id ?? ""}</td><td id="input${idStr}_mode"></td></tr>`;
    return str + `
<tr><td></td><td> UTC</td><td id="input${idStr}_utc"></td></tr>
<tr><td></td><td> position</td><td id="input${idStr}_pos"></td></tr>
<tr><td></td><td> quality</td><td><span id="input${idStr}_valid"></span> <span id="input${idStr}_hdop"></span> <span id="input${idStr}_gps_hdop"></span> <span id="input${idStr}_glo_hdop"></span></td></tr>
<tr><td></td><td> GPS</td><td id="input${idStr}_gps"></td></tr>
<tr><td></td><td> GLO</td><td id="input${idStr}_glo"></td></tr>
`;
}

function MakePageData(a_id) {
    const idStr = "gnss" + (a_id ?? "");
    const appName = "mx" + idStr;
    const configOut = sc_config.GetConfig().out;
    const fileLatest = sc_utils.GetOutFile(configOut.path, appName);
    if (fileLatest === undefined || !fileLatest.length)
        return null;
    const fileNameParsed = sc_utils.ParseFileName(fileLatest);
    if (fileNameParsed === undefined)
        return null;
    let data = {};
    const filePath = configOut.path + '/' + fileLatest;
    try {
        const gnss = sc_utils.ReadFileJSON(filePath);
        const status = sc_utils_gnss.GetStatus(gnss, fileNameParsed.utc);
        data.input = {};
        data.class = {};
        data.color = {};
        if (status.text !== "") {
            data.input[idStr + "_mode"] = status.text; // gnss.mode.toLowerCase();
            data.class[idStr + "_mode"] = status.text_class;
        }
        else {
            data.input[idStr + "_mode"] = gnss.mode.toLowerCase();
            data.class[idStr + "_mode"] = "";
        }
        data.input[idStr + "_utc"] = gnss.utc;
        data.input[idStr + "_pos"] = sc_utils_gnss.GetPosSpdShort(gnss);
        sc_utils_gnss.SetValid(data, idStr + "_valid", gnss.valid);
        sc_utils_gnss.SetHDOP(data, idStr + "_hdop", gnss.hdop);
        sc_utils_gnss.SetHDOP(data, idStr + "_gps_hdop", gnss.gps_hdop);
        sc_utils_gnss.SetHDOP(data, idStr + "_glo_hdop", gnss.glo_hdop);
        data.input[idStr + "_gps"] = sc_utils_gnss.GetSatsStrGPS(gnss);
        data.input[idStr + "_glo"] = sc_utils_gnss.GetSatsStrGLO(gnss);
        data.color[idStr] = status.color;
    } catch (err) {
        data = {};
        data.color = {}
        data.color[idStr] = "red";
    }
    data.utc_file = fileNameParsed.dateTime;
    return data;
}
