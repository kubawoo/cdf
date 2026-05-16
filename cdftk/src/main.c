#include <cdf.h>
#include <json.h>

JsonObject * read_cdfmodule() {
    String * filename = new(String, "cdfmodule.json");
    InputStream * io = new(FileInputStream, filename);
    REFCDEC(filename);

    JsonObjectBuilderEventsHandler * handler = REFCTMP(new(JsonObjectBuilderEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);
    int ret = call(parser, parse, io);
    REFCDEC(io);


    if(ret != CJSON_PARSE_SUCCESS) {
        REFCDEC(parser);
        return NULL;
    }


    JsonObject * jo = handler->object;
    REFCINC(jo);


    REFCDEC(parser);


    return jo;
}


int main(void) {
    Console * c = new(Console);


    JsonObject * cdf = read_cdfmodule();
    call(c, print_object, cdf);

    String * n = new(String, "name");
    String * name = call(cdf, get_value, n);
    REFCDEC(n);
    call(c, print_object, name);
    REFCDEC(name);

    REFCDEC(cdf);

    REFCDEC(c);
    return 0;
}
