
#include <AvTranscoder/properties/JsonWriter.hpp>
#include <AvTranscoder/properties/util.hpp>

namespace avtranscoder
{
namespace json
{

std::string build(PropertyMap& properties)
{
    json::JsonObjectStreamWriter writer;
    for(PropertyMap::iterator it = properties.begin(); it != properties.end(); ++it)
    {
        switch(it->second.getType())
        {
            case EPropertyValueKind::EPropertyValueKindString:
            {
                writer << std::make_pair(it->first.c_str(), it->second.getString().c_str());
                break;
            }
            case EPropertyValueKind::EPropertyValueKindInt:
            {
                writer << std::make_pair(it->first.c_str(), it->second.getInt());
                break;
            }
            case EPropertyValueKind::EPropertyValueKindUInt:
            {
                writer << std::make_pair(it->first.c_str(), it->second.getUInt());
                break;
            }
            case EPropertyValueKind::EPropertyValueKindFloat:
            {
                writer << std::make_pair(it->first.c_str(), it->second.getFloat());
                break;
            }
            case EPropertyValueKind::EPropertyValueKindDouble:
            {
                writer << std::make_pair(it->first.c_str(), it->second.getDouble());
                break;
            }
            case EPropertyValueKind::EPropertyValueKindBoolean:
            {
                writer << std::make_pair(it->first.c_str(), it->second.getBool());
                break;
            }
            case EPropertyValueKind::EPropertyValueKindRational:
            {
                Rational r = it->second.getRational();
                JsonObjectStreamWriter ratio;
                ratio << std::make_pair("num", r.num);
                ratio << std::make_pair("den", r.den);
                writer << std::make_pair(it->first.c_str(), ratio.build());
                break;
            }
        }
        
    }
    return writer.build();
}

}
}
