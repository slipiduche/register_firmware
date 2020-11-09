void loadMqttCommand(String datar)
{
    if (modo_nowc == 0)
    {
        DEBUG_PRINTLN("Ignored...");
    }
    else if (datar.startsWith("{\"REQUEST\":\"INFO\"}"))
    {  
        serverPoll=1;
    }
       
}
