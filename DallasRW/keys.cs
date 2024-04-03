using System.Text.Json.Nodes;

namespace DallasRW
{
    internal static class FileKeys
    {
        public static string[] GetKeyGroups(JsonObject jsonObj)
        {
            List<string> KeyGroups = new List<string>();

            try
            {
                foreach (var i in jsonObj)
                {
                    KeyGroups.Add(i.Key);
                }
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }

            return KeyGroups.ToArray();
        }

        public static bool Update(JsonObject jsonObj, string keyGroupPrev, string keyGroupNew, string keyLabelPrev, string keyLabelNew, string keyIDPrev, string keyIDNew)
        {
            try
            {
                Remove(jsonObj, keyGroupPrev, keyLabelPrev);
                return Add(jsonObj, keyGroupNew, keyLabelNew, keyIDNew);
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }
            return false;
        }

        public static bool Add(JsonObject jsonObj, string keyGroup, string keyLabel, string keyID)
        {
            try
            {
                foreach (var i in jsonObj)
                {
                    if (i.Key == keyGroup)
                    {
                        JsonObject JObjSub = i.Value?.AsObject()!;
                        JObjSub.Add(keyLabel, keyID);
                    }
                }
                return true;
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }
            return false;
        }

        public static void Remove(JsonObject jsonObj, string keyGroup, string keyLabel)
        {
            try
            {
                foreach (var i in jsonObj)
                {
                    if (i.Key == keyGroup)
                    {
                        JsonObject JObjSub = i.Value?.AsObject()!;
                        foreach (var j in JObjSub)
                        {
                            if (j.Key == keyLabel)
                            {
                                JObjSub.Remove(keyLabel);
                                return;
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }
        }

        public static void Remove(string filePath, string keyGroup, string keyLabel)
        {
            try
            {
                StreamReader sr = new StreamReader(filePath);
                string DataJSON = sr.ReadToEnd();
                sr.Close();
                JsonObject JObj = JsonNode.Parse(DataJSON)?.AsObject()!;
                Remove(JObj, keyGroup, keyLabel);
                File.WriteAllText(filePath, JObj.ToJsonString());
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }
        }
    }
}
