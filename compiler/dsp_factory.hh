/************************************************************************
 ************************************************************************
    FAUST compiler
	Copyright (C) 2016 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ************************************************************************
 ************************************************************************/

#ifndef __dsp_factory_base__
#define __dsp_factory_base__

#include <string>
#include <vector>
#include <ostream>

struct Meta;
class dsp_factory;
class dsp;

/*
 In order to better separate compilation and execution for dynamic backends (LLVM, interpreter, asm.js, WebAssembly).
 A dsp_factory_base* object will either be generated by the compiler from a dsp,
 or by reading an already compiled dsp (in LLVM IR or interpreter bytecode).
 */

class dsp_factory_base {
    
    public:
    
        virtual ~dsp_factory_base()
        {}
        
        virtual std::string getName() = 0;
        
        virtual std::string getSHAKey() = 0;
        virtual void setSHAKey(const std::string& sha_key) = 0;
        
        virtual std::string getDSPCode() = 0;
        virtual void setDSPCode(const std::string& code) = 0;
        
        virtual dsp* createDSPInstance(dsp_factory* factory) = 0;
        
        virtual void metadata(Meta* meta) = 0;
    
        virtual void write(std::ostream* out, bool binary = false, bool small = false) = 0;
    
        virtual std::vector<std::string> getDSPFactoryLibraryList() = 0;
    
        // Sub-classes will typically implement this method to create a factory from a stream
        static dsp_factory_base* read(std::istream* in) { return nullptr; }
  
};

class dsp_factory_imp : public dsp_factory_base {
    
    protected:
    
        std::string fName;
        std::string fSHAKey;
        std::string fExpandedDSP;
        std::vector<std::string> fPathnameList;
    
    public:
    
        dsp_factory_imp(const std::string& name,
                        const std::string& sha_key,
                        const std::string& dsp,
                        const std::vector<std::string>& pathname_list)
            :fName(name), fSHAKey(sha_key), fExpandedDSP(dsp), fPathnameList(pathname_list)
        {}
    
        dsp_factory_imp(const std::string& name,
                        const std::string& sha_key,
                        const std::string& dsp)
            :fName(name), fSHAKey(sha_key), fExpandedDSP(dsp)
        {}
        
        virtual ~dsp_factory_imp()
        {}
    
        std::string getName() { return fName; }
        
        std::string getSHAKey() { return fSHAKey; }
        void setSHAKey(const std::string& sha_key) { fSHAKey = sha_key; }
        
        std::string getDSPCode() { return fExpandedDSP; }
        void setDSPCode(const std::string& code) { fExpandedDSP = code; }
        
        virtual dsp* createDSPInstance(dsp_factory* factory) { return nullptr; }
        
        virtual void metadata(Meta* meta) {}
    
        virtual void write(std::ostream* out, bool binary = false, bool small = false) {}
    
        virtual std::vector<std::string> getDSPFactoryLibraryList() { return fPathnameList; }
 
};

class text_dsp_factory_aux : public dsp_factory_imp {
    
    protected:
    
        std::string fCode;
    
    public:
    
        text_dsp_factory_aux(const std::string& name,
                             const std::string& sha_key,
                             const std::string& dsp,
                             const std::vector<std::string>& pathname_list,
                             const std::string& code)
            :dsp_factory_imp(name, sha_key, dsp, pathname_list), fCode(code)
        {}
        
        virtual void write(std::ostream* out, bool binary = false, bool small = false)
        {
            *out << fCode;
        }
};

#endif