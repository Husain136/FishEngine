from mako.template import Template
import json

serialization_template_str = '''
/**************************************************
* auto generated by reflection system
**************************************************/

${headers}

namespace FishEngine
{
	${serialize_functions}
} // namespace FishEngine
'''
serialization_template = Template(serialization_template_str)

serialization_function_template_str = '''
	// ${T}
	template<>
	void Serialization::Serialize(OutputArchive& archive, const ${T} & value)
	{
		${serialize_seqs}
	}

	template<>
	void Serialization::Deserialize(InputArchive& archive, ${T} & value)
	{
		${deserialize_seqs}
	}
'''
serialization_function_template = Template(serialization_function_template_str)

def GenSerializationFunctions(classinfo):
    headers = []
    functions = []
    visited = set()
    def register(classname):
        if classname in visited:
            return
        c = classinfo[classname]
        visited.add(classname)

    for key in classinfo.keys():
        register(key)
        c = classinfo[key]
        headers.append(c['header_file'])
        serialize_seqs_list = [x for x in c['members'] if not x['NonSerializable']]
        serialize_seqs = ['archive << value.{0}; // {1}'.format(x['name'], x['type']) for x in serialize_seqs_list]
        # if 'parent' in c:
        #     p = 'Serialization::Serialize<{0}>(archive, value);'.format(c['parent'])
        #     serialize_seqs.insert(0, p)
        serialize_seqs = '\n\t\t'.join(serialize_seqs)
        deserialize_seqs = serialize_seqs.replace('<<', '>>')
        f = serialization_function_template.render(namespace=c['scope_prefix'], T=key, serialize_seqs=serialize_seqs, deserialize_seqs=deserialize_seqs)
        functions.append(f)

    headers = '\n'.join(['#include "../{}"'.format(x) for x in headers])
    serialize_functions = '\n'.join(functions)
    return serialization_template.render(headers = headers, 
        serialize_functions=serialize_functions)

#register_teamplate
componentInheritance_template_str = '''
static std::map<std::string, std::string> s_componentInheritance = {
    ${pairs}
};
'''

classname_template_str = '''

'''

def GenComponentInheritance(class_info):
    def IsComponent(name):
        #print(name)
        if name == "Component":
            return True
        if 'parent' not in class_info[name]:
            return False
        return IsComponent(class_info[name]['parent'])

    pairs = []
    for key in class_info.keys():
        if "Component" == key:
             pairs.append((key, ''))
        elif IsComponent(key):
            pairs.append((key, class_info[key]['parent']))
    pairs = ['{{"{0}", "{1}"}},'.format(x, y) for (x, y) in pairs]
    #print(pairs)
    print(Template(componentInheritance_template_str).render(pairs='\n\t'.join(pairs)))

if __name__ == "__main__":
    with open('temp/class.json') as f:
        class_info = json.loads(f.read())
    GenComponentInheritance(class_info)
    with open('../../Engine/Source/Runtime/generate/Class_Serialization.hpp', 'w') as f:
        f.write(GenSerializationFunctions(class_info))

