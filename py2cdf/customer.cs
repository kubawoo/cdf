import ooc.h

class Customer:

    def __init__(self, first_name : String, last_name : String):
        self.first_name = first_name
        self.last_name = last_name

    def to_string(self) -> String:
        return '%s %s' % (self.first_name, self.last_name)

    def get_first_name(self) -> String:
        return self.first_name


