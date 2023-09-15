/*Copyright 2014 Francisco Alvaro

This file is part of SESHAT.

  SESHAT is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  SESHAT is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with SESHAT.  If not, see <http://www.gnu.org/licenses/>.


This file is a modification of the RNNLIB original software covered by
the following copyright and permission notice:

*/
/*Copyright 2009,2010 Alex Graves

This file is part of RNNLIB.

RNNLIB is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RNNLIB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RNNLIB.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef _INCLUDED_DataExporter_h
#define _INCLUDED_DataExporter_h

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "ConfigFile.hpp"
#include "Helpers.hpp"
#include "Named.hpp"
#include "SeqBuffer.hpp"

#define SAVE(x) (save(x, #x))
#define DISPLAY(x) (display(x, #x))

struct Val {
    virtual void print(std::ostream& out) const = 0;
    virtual bool load(std::istream& in, std::ostream& out = std::cout)
    {
        return false;
    }
    virtual ~Val() = default;
};

static std::ostream& operator<<(std::ostream& out, const Val& v)
{
    v.print(out);
    return out;
}

static std::istream& operator>>(std::istream& in, Val& v)
{
    v.load(in);
    return in;
}

template<typename R>
struct RangeVal : public Val {
    // data
    R range;

    // functions
    explicit RangeVal(const R& r)
        : range(r) { }

    void print(std::ostream& out) const
    {
        out << std::size(range) << " ";
        print_range(out, range);
    }

    bool load(std::istream& in, std::ostream& out = std::cout)
    {
        int size;
        if (in >> size && size == std::size(range)) {
            if (!(in >> range)) {
                out << "ERROR unable to read from stream" << std::endl;
                return false;
            }
            return true;
        } else {
            out << "ERROR saved size " << size << " != current size "
                << std::size(range) << std::endl;
            return false;
        }
    }
};

template<typename T>
struct ParamVal : public Val {
    // data
    T& param;

    // functions
    explicit ParamVal(T& p)
        : param(p) { }

    void print(std::ostream& out) const
    {
        out << param;
    }

    bool load(std::istream& in, std::ostream& out = std::cout)
    {
        return (bool)(in >> param);
    }
};

template<typename T>
struct SeqBufferVal : public Val {
    // data
    const SeqBuffer<T>& array;
    const std::vector<std::string>* labels;

    // functions
    SeqBufferVal(const SeqBuffer<T>& a, const std::vector<std::string>* labs = 0)
        : array(a)
        , labels(labs) { }

    void print(std::ostream& out) const
    {
        if (!array.empty()) {
            if (labels) {
                out << "LABELS: " << *labels << std::endl;
                // print_range(out, *labels);
                // out << std::endl;
            }
            out << array;
        }
    }
};

class DataExporter;
class Val;

typedef std::map<std::string, Val*>::const_iterator CONST_VAL_IT;
typedef std::map<std::string, Val*>::iterator VAL_IT;
typedef std::map<std::string, DataExporter*>::const_iterator CONST_EXPORT_IT;
typedef std::map<std::string, DataExporter*>::iterator EXPORT_IT;
typedef std::pair<const std::string, DataExporter*> PSPDE;
typedef std::pair<const std::string, Val*> PSPV;

struct DataExportHandler {
    // data
    std::map<std::string, DataExporter*> dataExporters;

    // functions
    void save(std::ostream& out) const;
    void load(ConfigFile& conf, std::ostream& out = std::cout);
    void display(const std::string& path) const;
};

static std::ostream& operator<<(std::ostream& out, const DataExportHandler& de)
{
    de.save(out);
    return out;
}

struct DataExporter : public Named {
    // data
    std::map<std::string, Val*> saveVals;
    std::map<std::string, Val*> displayVals;

    // functions
    explicit DataExporter(const std::string& name, DataExportHandler* DEH)
        : Named(name)
    {
        DEH->dataExporters[name] = this;
    }
    ~DataExporter()
    {
        delete_map(displayVals);
        delete_map(saveVals);
    }
    void save(std::ostream& out) const
    {
        for (CONST_VAL_IT it = saveVals.begin(); it != saveVals.end(); ++it) {
            out << name << "_" << it->first << " " << *(it->second) << std::endl;
        }
    }
    bool load(ConfigFile& conf, std::ostream& out = std::cout)
    {
        LOOP(PSPV & val, saveVals)
        {
            std::string lookupName = name + "_" + val.first;
            std::string displayName = name + "." + val.first;
            if (verbose) {
                out << "loading " << displayName << std::endl;
            }
            std::map<std::string, std::string>::iterator stringIt = conf.params.find(lookupName);
            if (stringIt == conf.params.end()) {
                out << "WARNING: unable to find '" << displayName << "'" << std::endl;
            } else {
                std::istringstream ss(stringIt->second);
                if (val.second->load(ss, out)) {
                    conf.params.erase(stringIt);
                } else {
                    out << "WARNING: unable to load '" << displayName << "'" << std::endl;
                    return false;
                }
            }
        }
        return true;
    }
    void delete_val(std::map<std::string, Val*>& vals, const std::string& name)
    {
        if (in(vals, name)) {
            delete vals[name];
            vals.erase(name);
        }
    }
    template<typename T>
    void save(T& param, const std::string& name)
    {
        delete_val(saveVals, name);
        saveVals[name] = new ParamVal<T>(param);
    }
    template<typename R>
    void save_range(const R& range, const std::string& name)
    {
        delete_val(saveVals, name);
        saveVals[name] = new RangeVal<R>(range);
    }
    template<typename T>
    void display(
        const SeqBuffer<T>& array, const std::string& name,
        const std::vector<std::string>* labels = 0)
    {
        delete_val(displayVals, name);
        displayVals[name] = new SeqBufferVal<T>(array, labels);
    }
};

static std::ostream& operator<<(std::ostream& out, const DataExporter& d)
{
    d.save(out);
    return out;
}

#endif
