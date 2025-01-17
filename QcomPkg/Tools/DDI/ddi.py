
#=============================================================================
# FILE:         ddi.py
#
# OVERVIEW:     DDI automation script
#
#               Copyright (c) 2018,2019 Qualcomm Technologies, Inc.
#               All Rights Reserved.
#               Qualcomm Technologies Proprietary and Confidential.
#=============================================================================
# ----------------------------------
# Portions
#
# 1.	Copyright Michael Foord 2004
# 2.	Released subject to the BSD License
# 3.	Please see http://www.voidspace.org.uk/python/license.shtml
# Copyright (c) 2003-2010, Michael Foord
# All rights reserved.
# E-mail : fuzzyman AT voidspace DOT org DOT uk
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following
# disclaimer in the documentation and/or other materials provided
# with the distribution.
#
# Neither the name of Michael Foord nor the name of Voidspace
# may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ------------------------------

import argparse
import os
import json
import struct
import shutil
import tempfile
import subprocess
import sys
import re
import time
import glob
import fnmatch

import logging
import xml.etree.ElementTree as ET

# TODO: DEBUG
import pprint
DEBUG = False

def onerror(func, path, exc_info):
    """
    Error handler for ``shutil.rmtree``.

    If the error is due to an access error (read only file)
    it attempts to add write permission and then retries.

    If the error is for another reason it re-raises the error.

    Usage : ``shutil.rmtree(path, onerror=onerror)``
    """
    import stat
    if not os.access(path, os.W_OK):
        # Is the error an access error ?
        os.chmod(path, stat.S_IWUSR)
        func(path)
    else:
        raise Exception

def genremove(logger, rmpath):
    try:
        if os.path.isdir(rmpath):
            shutil.rmtree(rmpath, onerror=onerror)
        else:
            if os.path.isfile(rmpath):
                os.remove(rmpath)
            else:
                logger.debug(" %s is not present , skip delete", rmpath)
        time.sleep(1)
    except Exception as e:
        logger.debug(" could not remove path %s", rmpath)
        logger.debug(e)

def run_command(logger, cmd, errorkey=None, failonerror=True, Dly=0):
    errorcheck = False
    logger.info(" Running cmd: %s", cmd)
    logger.debug(cmd.split(" "))
    p = subprocess.Popen(cmd.split(" "), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    for c in iter(p.stdout.readline, ""):
        if errorkey is not None and errorkey in c:
            errorcheck = True
        logger.debug(c)

    time.sleep(Dly)

    logger.debug(" popo1: %s", cmd)
    logger.debug(" popo2: %s", cmd)        
    logger.debug(" popo3: %s", cmd)

    supressMsg = False
    subprocess.Popen("TASKLIST /FI \"PID eq {pid}\"".format(pid=p.pid), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    for c in iter(p.stdout.readline, ""):
        if "No tasks" in c:
            supressMsg = True
        logger.debug(c)

    if supressMsg:
        subprocess.Popen("TASKKILL /F /PID {pid} /T".format(pid=p.pid))

    if errorcheck:
        if failonerror:
            logger.error("[ERROR] found when running command, system_exit")
            sys.exit()
        logger.warning("[WARNING] Run command end with errorcheck=True")
        return False
    else:
        logger.debug(" Run command successfully")
        return True


class FlashInterface(object):
    def __init__(self, output_dir, meta_build, port=None, boot_build=None, partition_xmls=None, flash_type=None):
        self.logger = logging.getLogger("ddi.FlashInterface")
        self.output_dir = output_dir
        if flash_type is None: #default to UFS
            flash_type = "ufs"
        if "nor" in flash_type:
            self.flash_type = "spinor"
            self.meta_flash_type = "nor"
        else:
            self.flash_type = flash_type
            self.meta_flash_type = flash_type

        self.partition_xmls = partition_xmls
        self.meta_build = meta_build
        partition_xml = None
        if self.meta_build is None:
            for xml in self.partition_xmls:
                if os.path.basename(xml) == "partition.xml":
                    partition_xml = xml
        else:
            partition_xml = os.path.join(self.meta_build, "common", "Config", self.meta_flash_type, "partition.xml")
        if partition_xml is None:
            self.logger.error("[ERROR] could not file partition.xml file in meta build or partition xml list")
            sys.exit()
        part_xml_tree = ET.parse(partition_xml)
        self.part_xml_root = part_xml_tree.getroot()

        if boot_build is None:
            boot_build = os.path.dirname(__file__).split("boot_images")[-2]
        self.boot_build = boot_build

        self.fh_loader = os.path.join(self.boot_build, "boot_images", "QcomPkg", "Tools", "storage", "fh_loader", "fh_loader.exe")
        self.qsaharaserver = os.path.join(self.boot_build, "boot_images", "QcomPkg", "Tools", "storage", "fh_loader", "QSaharaServer.exe")
        self.lsusb = os.path.join(self.boot_build, "boot_images", "QcomPkg", "Tools", "storage", "fh_loader", "lsusb.exe")

        if port is None:
            self.port = self.get_edl_port_num()
            if(self.port == -1):
                sys.exit()
        else:
            self.port = port

    # this function will set the following class vars:
    # self.lun_num
    # self.start_sector
    # self.num_sectors
    # self.file_name
    def __set_partition_details(self, partition):
        partition = partition.lower()
        self.part_xml_root = ET.fromstring(ET.tostring(self.part_xml_root).lower())

        self.lun_num = None
        for partition_name in [partition+"_a", partition]:
            if self.lun_num is not None:
                break
            for i,physical_partition in enumerate(self.part_xml_root.findall("./physical_partition")):
                if physical_partition.find("./partition[@label='%s']" %(partition_name)) is not None:
                    self.lun_num = i
                    partition = partition_name
                    break

        if self.lun_num is None:
            self.logger.error("[ERROR] could not find partition %s in the partition xml", partition)
            sys.exit()

        filename = "rawprogram%d.xml" %(self.lun_num)
        rawprogram_xml = None
        if self.meta_build is None:
            for xml in self.partition_xmls:
                if os.path.basename(xml) == filename:
                    rawprogram_xml = xml
        else:
            rawprogram_xml = os.path.join(self.meta_build, "common", "build", self.meta_flash_type, "rawprogram%d.xml" %(self.lun_num))
        if rawprogram_xml is None:
            self.logger.error("[ERROR] could not file %s file in meta build or partition xml list", filename)
            sys.exit()

        rawprogram_xml_tree = ET.parse(rawprogram_xml)
        rawprogram_xml_root = rawprogram_xml_tree.getroot()
        rawprogram_xml_root = ET.fromstring(ET.tostring(rawprogram_xml_root).lower())
        elm = rawprogram_xml_root.find("./program[@label='%s']" %(partition))
        self.start_sector = elm.attrib["start_sector"]
        self.num_sectors = elm.attrib["num_partition_sectors"]
        self.file_name = elm.attrib["filename"]

        if self.file_name == "":
            self.file_name = "%s.bin" %(partition)

        self.logger.debug("partition: %s", partition)
        self.logger.debug("filename: %s", self.file_name)
        self.logger.debug("start_sector: %s", self.start_sector)
        self.logger.debug("num_sectors: %s", self.num_sectors)


    def __find_com_port(self, identifire):
        poll_count = 0
        poll_completed = False
        p = subprocess.Popen(self.lsusb, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        comport_list = []
        while(True):
            retcode = str(p.poll())
            if (poll_completed is True) or (poll_count == 10000):
                break #Breaking loop here
            if(retcode is not None):
                portname = p.stdout.readline()
                poll_completed =  ( len(portname) <= 0 )
                self.logger.debug("Port : " + str(portname))
                if identifire is None or identifire in portname:
                    match_comport = re.search('\(COM([0-9]+)\)', portname)
                    if match_comport:
                        self.logger.debug(match_comport.group(1))
                        comport_list.append( match_comport.group(1))
            poll_count += 1

        return comport_list

    def get_edl_port_num(self, retry_count=60, forever=False):
        edl_ports = self.__find_com_port('QDLoader')
        self.logger.info("Wait until Device is connected....")
        while(len(edl_ports)<=0):
            edl_ports = self.__find_com_port('QDLoader')
            time.sleep(5)
            if not forever:
                retry_count -= 1
                if(retry_count == 0):
                    self.logger.error("[ERROR] Unable to detect MTP device EDL/QDLoader mode")
                    return -1
                    #sys.exit()

        self.logger.info("device EDL/QDLoader port %s is detected...",edl_ports[0])
        return edl_ports[0]


    def reset_device(self):
        cmd = "%s --port=\\\\.\COM%s --memoryname=%s --reset --noprompt" % (self.fh_loader, self.port, self.flash_type)
        run_command(self.logger, cmd, errorkey="ERROR", Dly=10)


    # device must first be in EDL mode
    def load_dp_lite(self, dp_lite):
        # check if DP is already loaded on the device
        cmd = "%s --port=\\\\.\COM%s --nop --noprompt" %(self.fh_loader, self.port)
        if not run_command(self.logger, cmd, errorkey="ERROR", failonerror=False):
            cmd = "%s -k -u %s -s 13:prog_firehose_lite.elf -b %s" %(self.qsaharaserver, self.port, os.path.join(os.path.dirname(dp_lite), ""))
            run_command(self.logger, cmd, errorkey="ERROR")


    def read_partition(self, partition):
        if partition is "partition_table":
            #TODO:valid for gpt but not for MDM MIBIB
            self.lun_num = 0
            self.start_sector = 0
            self.num_sectors = 40
            self.file_name = partition
        else:
            self.__set_partition_details(partition)

        genremove(self.logger, os.path.join(self.output_dir, self.file_name))

        os.chdir(self.output_dir)
        cmd = "%s --port=\\\\.\COM%s --convertprogram2read --sendimage=%s --start_sector=%s --lun=%s --num_sectors=%s --noprompt --zlpawarehost=1 --memoryname=%s" %(self.fh_loader, self.port, self.file_name, self.start_sector, self.lun_num, self.num_sectors, self.flash_type)
        run_command(self.logger, cmd, errorkey="ERROR")

        return os.path.join(self.output_dir, self.file_name)


    def write_partition(self, partition, file_path):
        self.__set_partition_details(partition)
        cmd = "%s --port=\\\\.\COM%s --sendimage=%s --search_path=%s --start_sector=%s --lun=%s --num_sectors=%s --noprompt --zlpawarehost=1 --memoryname=%s" %(self.fh_loader, self.port, os.path.basename(file_path), os.path.dirname(file_path), self.start_sector, self.lun_num, self.num_sectors, self.flash_type)
        run_command(self.logger, cmd, errorkey="ERROR")


#TODO: run-time auto-detect
DDILibTargetSettings = {
    "SDM845" : {
        "topology" : {"NUM_CH" : 4, "NUM_CS" : 2},
        "OSPathPkg" : "SDM845Pkg",
        "OSPathDSFPkg" : "SDM845Pkg",
        "LP4": "",
        "LP5": "",
    },
    "SDM670" : {
        "topology" : {"NUM_CH" : 2, "NUM_CS" : 2},
        "OSPathPkg" : "SDM670Pkg",
        "OSPathDSFPkg" : "SDM670Pkg",
        "LP4": "",
        "LP5": "",
    },
    "SDM855" : {
        "topology" : {"NUM_CH" : 4, "NUM_CS" : 2},
        "OSPathPkg" : "",
        "OSPathDSFPkg" : os.path.join("SDMPkg","855"),
        "LP4": "",
        "LP5": "",
    },
    "SDM1000" : {
        "topology" : {"NUM_CH" : 8, "NUM_CS" : 2},
        "OSPathPkg" : "",
        "OSPathDSFPkg" : os.path.join("SDMPkg","1000"),
        "LP4": "",
        "LP5": "",
    },
    "SM8250" : {
        "topology" : {"NUM_CH" : 4, "NUM_CS" : 2},
        "OSPathPkg" : "",
        "OSPathDSFPkg" : os.path.join("SocPkg","8250"),
        "LP4": "SM8250LP4",
        "LP5": "SM8250LP5",
        
    },
}


class DCBLib(object):
    # class globals

    binaries_ordered_by_header_index = ['DDR_RUNTIME',
                                        'AOP_PARAMS',
                                        'TRAINING_PARAMS',
                                        'ECDT_PRE_DEVICE_INIT',
                                        'ECDT_POST_DEVICE_INIT',
                                        'DDRSS',
                                        'NOC',
                                        'SHRM',
                                        'MCCC_MSTR',
                                        'MCCC_SLV',
                                        'DQ_PHY',
                                        'CA_PHY',
                                        'DDR_CC',
                                        'MC',
                                        'LLCC',
                                        'SAVE_RESTORE',
                                        'SHRM_DUMP',
                                        'DDI_INPUT']
    dcbBinDict = {}
    checksum = 0
    dcb_size = 0
    dsf_version = 0
    DCB_MAX_SIZE = ((1024 * 13) + 4)

    @staticmethod
    def bytes_crc32(bytes):
        CRC_POLYNOMIAL = 0x04C11DB7
        CRC_INITVAL = 0xFFFFFFFF
        CRC_XOROUT = 0xFFFFFFFF
        result = CRC_INITVAL
        for octet in bytes:
            if type(octet) is str:
                octet = struct.unpack("B", octet)[0]
            for bit in range(0,8):
                if (octet >> 7) ^ (result >> 31):
                    result = ((result << 1) & 0xFFFFFFFF) ^ CRC_POLYNOMIAL
                else:
                    result = (result << 1) & 0xFFFFFFFF
                octet = (octet << 1) & 0xFF
        result = result ^ CRC_XOROUT
        return result

    def PopulateDcbBinDictFromExt(self, dcb, binnames):
        # populate the dict from the existing dcb, override with binnames
        with open(dcb, "rb") as dcb_f:
            self.checksum = struct.unpack("<I", dcb_f.read(4))[0]
            self.dcb_size = struct.unpack("<I", dcb_f.read(4))[0]
            self.dsf_version = struct.unpack("<I", dcb_f.read(4))[0]

            header_offset = 12
            for bin in self.binaries_ordered_by_header_index:
                bin_off = struct.unpack("<H", dcb_f.read(2))[0]
                bin_size = struct.unpack("<H", dcb_f.read(2))[0]
                header_offset += 4

                # print("bin check: "+bin)
                if bin in binnames:
                    with open(binnames[bin], "rb") as bin_f:
                        self.dcbBinDict[bin] = bin_f.read()
                else:
                    dcb_f.seek(bin_off)
                    self.dcbBinDict[bin] = dcb_f.read(bin_size)
                    dcb_f.seek(header_offset)

    def WriteDcbBinary(self, filename=None, output=None):

        header_max_size = 100

        # Header bytes object
        header = bytes()

        # Data bytes object
        data = bytes()

        # Data starts at end of header
        offset = header_max_size

        # Loop through each binary section of DCB
        for bin in self.binaries_ordered_by_header_index:
            # Update header
            size = len(self.dcbBinDict[bin])
            header += struct.pack("<H", offset)
            header += struct.pack("<H", size)
            offset += size

            # Update data
            data += self.dcbBinDict[bin]

            # Align each section of DCB to 4 byte boundary
            remainder = offset % 4
            if (remainder != 0):
                remainder = 4 - remainder
            for _ in range(remainder):
                data += struct.pack("<B", 0)
            offset += remainder

        header = struct.pack("<I", self.dsf_version) + header
        # Size of DCB including header
        size = len(data) + header_max_size
        header = struct.pack("<I", size) + header

        # CRC set to zero as placeholder for CRC calculation
        header = struct.pack("<I", 0) + header

        # Pad header with zeros
        header = header.ljust(header_max_size, b'\0')

        # Combine header and data
        binary = header + data

        # Pad binary with zeros
        binary = binary.ljust(self.DCB_MAX_SIZE, b'\0')

        # Calculate CRC on entire DCB binary
        crc = self.bytes_crc32(binary)

        # Replace placeholder CRC
        binary = struct.pack("<I", crc) + binary[4:]

        # Print information on which XL file is being processed and resulting binary
        # Print CRC and size information
        self.logger.info("[INFO] DCB {:s}, CRC 0x{:08X}, {:d}B free ({:d}/{:d} used)".format(
            os.path.basename(filename),
            crc,
            self.DCB_MAX_SIZE - size,
            size,
            self.DCB_MAX_SIZE))

        # Write to file
        with open(os.path.join(output, filename), "wb") as f:
            f.write(binary)

        # Write binaries generated for debug
        if DEBUG:
            for bin in self.binaries_ordered_by_header_index:
                # Write to file
                with open("output_{:s}.bin".format(bin), "wb") as f:
                    f.write(self.dcbBinDict[bin])



class DDILib(object):
    # class globals
    BIN_NAMES = ["DDI_INPUT", "ECDT_POST_DEVICE_INIT", "ECDT_PRE_DEVICE_INIT"]
    NUM_CH = 4 #default , overwritten by target specific config at init
    NUM_CS = 2 #default , overwritten by target specific config at init
    NUM_BYTE = 2
    NUM_PHASE = 2
    NUM_BIT = 8

    def __init__(self, output_dir, prot_xml=None, boot_build=None, target=None):
        self.logger = logging.getLogger("ddi.DDILib")
        if target is None: #default
            target = "SDM845"
            self.logger.debug("no target specified, use default")
        self.target = target
        self.logger.info("DDI Lib target config %s", self.target)
        DDILib.NUM_CH = DDILibTargetSettings[target]["topology"]["NUM_CH"]
        DDILib.NUM_CS = DDILibTargetSettings[target]["topology"]["NUM_CS"]

        if boot_build is None:
            boot_build = os.path.dirname(os.path.realpath(__file__)).split("boot_images")[-2]
        self.boot_build = boot_build

        # parse protocol xml into data structure
        if prot_xml==None:
            ddi_tools_path = os.path.join(self.boot_build, "boot_images", "QcomPkg", DDILibTargetSettings[target]["OSPathPkg"], "Tools", "DDI")
            if os.path.exists(os.path.join(ddi_tools_path, "ddi_protocol_config_internal.xml")):
                prot_xml = os.path.join(ddi_tools_path, "ddi_protocol_config_internal.xml")
            else:
                prot_xml = os.path.join(ddi_tools_path, "ddi_protocol_config.xml")
        self.logger.info("parsing: %s", prot_xml)
        self.prot_xml_tree = ET.parse(prot_xml)
        self.prot_root = self.prot_xml_tree.getroot()

        self.output_dir = output_dir
        self.xblconfig_tools_loaded = False
        self.force_copy = False

        # populate command lookup
        self.command_lookup = {}
        for cmd in self.prot_root.findall("./Commands/Command"):
            cmd_id = DDILib.__convert_to_int(cmd.find("CommandID").text)
            self.command_lookup[cmd_id] = cmd

        #self.import_dsf = None

    def __get_xblconfig_tools(self):
        # TODO: import tools
        try:
            self.logger.debug("__get_xbl_config_tools, init")
            tools_path = os.path.join(self.boot_build, "boot_images", "QcomPkg", "Tools", "XBLConfig")
            dest_path =os.path.join(self.output_dir, "xblconfig")
            if(self.force_copy or (not os.path.exists(dest_path))):
                genremove(self.logger, dest_path)
                shutil.copytree(tools_path, dest_path)
                self.logger.info("XBLConfig tools copied to: %s", dest_path)
            else:
                self.logger.info("NOT COPY XBLConfig tools to: %s", dest_path)
                self.logger.info("reason: folder exist: " + str(os.path.exists(dest_path)) + ", force_copy: " + str(self.force_copy))
                
            tools_path = os.path.join(self.boot_build, "boot_images", "QcomPkg", "Tools", "sectools")
            dest_path =os.path.join(self.output_dir, "sectools")
            if(self.force_copy or (not os.path.exists(dest_path))):
                genremove(self.logger, dest_path)
                shutil.copytree(tools_path, dest_path)
                self.logger.info("sectools copied to: %s", dest_path)
            else:
                self.logger.info("NOT COPY sectools to: %s", dest_path)
                self.logger.info("reason: folder exist: " + str(os.path.exists(dest_path)) + ", force_copy: " + str(self.force_copy))
        except Exception as e:
            self.logger.debug(e)
            self.logger.debug("__get_xbl_config_tools, exception on copy tool.")
        
        try:        
            genremove(self.logger, os.path.join(self.output_dir, "decompose"))
            os.makedirs(os.path.join(self.output_dir, "decompose"))
            genremove(self.logger, os.path.join(self.output_dir, "compose"))
            os.makedirs(os.path.join(self.output_dir, "compose"))
            self.xblconfig_tools_loaded = True
            self.logger.debug("__get_xbl_config_tools, end")
        except Exception as e:
            self.logger.debug(e)
            self.logger.debug("__get_xbl_config_tools, exception on removing decompose or compose")
            return False

    struct_type_conversion = {
        "uint8":"B",
        "uint16":"H",
        "int32":"i",
        "uint32":"I",
        "uint64":"Q",
        "int64":"q",
    }

    int_to_size_conversion = {
        "uint8":1,
        "uint16":2,
        "int32":4,
        "uint32":4,
        "uint64":8,
        "int64":8
    }

    @staticmethod
    def __nameValue_to_dict(nvArr):
        # convert name value object array to a dictionary
        new_dict = {}
        for entry in nvArr:
            new_dict[entry["Name"]] = entry["Value"]
        
        return new_dict

    @staticmethod
    def __convert_to_int(s):
        if s is None:
            return None
        if isinstance(s, (int, long)):
            return s
        if "0x" in s:
            return int(s, 16)
        else:
            return int(s)

    def __parse_option(self, node, val):
        name = node.find("Name").text
        val_check = DDILib.__convert_to_int(node.find("Value").text)

        self.logger.debug("Option Name: %s", name)
        self.logger.debug("Option val: %d", val_check)
        if val != val_check:
            return False
        else:
            return True

    def __parse_param(self, node, data=None):
        ptype = node.find("Type").text
        ptypef = self.struct_type_conversion[ptype]
        name = node.find("Name").text
        default = node.find("Default")
        ret = DDILib.__convert_to_int(data)
        self.logger.debug("Name: %s", name)
        self.logger.debug("Type: %s (%s)", ptype, ptypef)

        if default is not None:
            default = default.text
            self.logger.debug("Default: %s", default)

            if data is None:
                ret = DDILib.__convert_to_int(default)
        elif ret is None:
            raise ValueError("no input provided and not default set")

        self.logger.debug("Set Val: %d", ret)

        if name == "Padding":
            out = ""
            for b in range(ret):
                out += struct.pack("<B", 0)
            return out

        if node.find("Options") is not None:
            check = False
            for option in node.findall("./Options/*"):
                if self.__parse_option(option, ret):
                    check = True
            if not check:
                raise ValueError("input value does not match option")
        
        if node.find("BitFields") is not None:
            numbits = len(node.findall("./BitFields/BitField"))
            test = (1 << numbits) - 1
            if ret > test:
                self.logger.debug("BitField test: 0x%x val: 0x%x", test, ret)
                raise ValueError("Using an invalid bit field")
        
        return struct.pack("<"+ptypef, ret)

    def __parse_paramarray(self, node, data):
        self.logger.debug("Parsing ParamArray")
        self.logger.debug(pprint.pformat(data))
        ret = ""
        size = ""
        ptype = node.find("Type").text
        ptypef = self.struct_type_conversion[ptype]
        if data is not None and "Size" in data:
            size = self.__parse_param(node, data=data["Size"])
        else:
            size = self.__parse_param(node)
        ret += size
        size = struct.unpack("<"+ptypef, size)[0]
        self.logger.debug("Size: %d", size)

        ptypef = self.struct_type_conversion["uint64"]
        for i in range(size):
            cur_data = DDILib.__convert_to_int(data["Data"][i])
            self.logger.debug("Data: 0x%x", cur_data)
            ret += struct.pack("<"+ptypef, cur_data)

        return ret

    def __parse_command(self, node, data):
        ret = ""
        cid = DDILib.__convert_to_int(data["CommandID"])
        self.logger.debug("CommandID: %d", cid)
        cnode = node.find("./Command[CommandID='%d']" %(cid))

        # log the command ID
        ret += self.__parse_param(cnode, data=cid)

        # go through each param, fill in the default, and override if value in config file

        # convert input params to dict
        data_input = {}
        if "Input" in data:
            if data["Input"]["Params"] is not None:
                data_input = DDILib.__nameValue_to_dict(data["Input"]["Params"])
            if data["Input"]["ParamArray"] is not None:
                data_input[data["Input"]["ParamArray"]["Name"]] = data["Input"]["ParamArray"]

        nodelist = cnode.findall("./Input/Params/*") + cnode.findall("./Input/ParamArray")
        for param in nodelist:
            name = param.find("Name").text
            if name in data_input:
                in_param = data_input[name]
            else:
                in_param = None
            if param.tag == "ParamArray":
                ret += self.__parse_paramarray(param, data=in_param)
            else:
                ret += self.__parse_param(param, data=in_param)
        
        return ret

    def gen_ddi_input(self, test_config):
        new_ddi_path = os.path.join(self.output_dir, "DDI_INPUT.bin")
        with open(test_config) as f_tc:
            tc_dict = json.load(f_tc)
            with open(os.path.join(self.output_dir, "DDI_INPUT.bin"), "wb") as f:
                # write commands header
                cur_dict = DDILib.__nameValue_to_dict(tc_dict["Commands_Header"])
                for param in self.prot_root.findall("./Commands_Header/*"):
                    name = param.find("Name").text
                    self.logger.debug("Param Name: %s", name)
                    if name == "Number of Commands":
                        ret = self.__parse_param(param, data=len(tc_dict["Commands"]))
                    elif name in cur_dict:
                        ret = self.__parse_param(param, data=cur_dict[param.find("Name").text])
                    else:
                        ret = self.__parse_param(param)
                    f.write(ret)

                # write individual commands
                cmds_node = self.prot_root.find("Commands")
                for cmd in tc_dict["Commands"]:
                    ret = self.__parse_command(cmds_node, cmd)
                    f.write(ret)
                
                input_size = f.tell()
                max_input = DDILib.__convert_to_int(self.prot_root.find("./Meta/InputSize").text)
                if input_size > max_input:
                    raise ValueError("DDI input size is larger than available size. input: 0x%x available: 0x%x", input_size, max_input)

                # pad the input file to the defined input size 
                remaining = max_input - input_size
                if remaining > 0:
                    f.write("\0"*remaining)

        return new_ddi_path
    
    def clear_ddi_output_bin(self, ddr_bin_path, clear_all=False):
        new_ddr_bin = os.path.join(self.output_dir, "ddr_edit.bin")
        with open(ddr_bin_path, "rb") as ddr_bin_f:
            output_offset = DDILib.__convert_to_int(self.prot_root.find("./Meta/OutputOffset").text)
            output_size = DDILib.__convert_to_int(self.prot_root.find("./Meta/OutputSize").text)
            with open(new_ddr_bin, "wb") as new_ddr_bin_f:
                if clear_all:
                    statout = os.stat(ddr_bin_path)
                    new_ddr_bin_f.write("\0"*statout.st_size)
                else:
                    new_ddr_bin_f.write(ddr_bin_f.read(output_offset))
                    new_ddr_bin_f.write("\0"*output_size)

        return new_ddr_bin 
    
    def get_ddi_output_bin(self, ddr_bin_path):
        ddi_path = os.path.join(self.output_dir, "ddi_output.bin")
        with open(ddr_bin_path, "rb") as ddr_bin_f:
            output_offset = DDILib.__convert_to_int(self.prot_root.find("./Meta/OutputOffset").text)
            output_size = DDILib.__convert_to_int(self.prot_root.find("./Meta/OutputSize").text)
            ddr_bin_f.seek(output_offset + 4) #FIXME:skip the 4 bytes of ddi output cookie
            with open(ddi_path, "wb") as ddi_bin_f:
                ddi_bin_f.write(ddr_bin_f.read(output_size))

        return ddi_path

    traindata = None
    def __read_param(self, node, cmd_obj, file):
        name = node.find("Name").text
        ptypef = node.find("Type").text
        size = DDILib.int_to_size_conversion[ptypef]
        ptypef = self.struct_type_conversion[ptypef]
        value = struct.unpack("<"+ptypef, file.read(size))[0]
        value = DDILib.__convert_to_int(value)
        self.logger.debug("Name: %s", name)
        self.logger.debug("Value: %d", value)
        

        if name == "Test Status":
            cmd_obj["TestStatus"] = value
        
        elif name == "Memory Type":
            cmd_obj["MemoryType"] = value
            
        # read in the training record packet and override plot record at trained point
        elif node.tag == "ParamArray" and name == "Train Record Packet":
            self.traindata = range(DDILib.NUM_CH)
            for ch in range(DDILib.NUM_CH):
                self.traindata[ch] = range(DDILib.NUM_CS)
                for cs in range(DDILib.NUM_CS):
                    self.traindata[ch][cs] = range(DDILib.NUM_BYTE)
                    for byte in range(DDILib.NUM_BYTE):
                        self.traindata[ch][cs][byte] = {}
                        self.traindata[ch][cs][byte]["vref"] = DDILib.__convert_to_int(struct.unpack("<B", file.read(1))[0])
                        self.traindata[ch][cs][byte]["coarse"] = DDILib.__convert_to_int(struct.unpack("<B", file.read(1))[0])
                        self.traindata[ch][cs][byte]["fine"] = DDILib.__convert_to_int(struct.unpack("<B", file.read(1))[0])
            import csv
            train_path = os.path.join(self.output_dir, "trained_values.csv")
            genremove(self.logger, train_path)
            with open(train_path, "wb") as train_csv:
                fieldnames = ["ch", "cs", "byte", "vref", "coarse", "fine"]
                writer = csv.DictWriter(train_csv, fieldnames=fieldnames, quoting=csv.QUOTE_NONNUMERIC, )
                writer.writeheader()
                row = {}
                for ch in range(DDILib.NUM_CH):
                    row["ch"] = ch
                    for cs in range(DDILib.NUM_CS):
                        row["cs"] = cs
                        for byte in range(DDILib.NUM_BYTE):
                            row["byte"] = byte
                            row["vref"] = self.traindata[ch][cs][byte]["vref"]
                            row["coarse"] = self.traindata[ch][cs][byte]["coarse"]
                            row["fine"] = self.traindata[ch][cs][byte]["fine"]
                            writer.writerow(row)

        # save a plot record packet to a new file
        elif node.tag == "ParamArray" and name == "Plot Record Packet":
            import csv
            plotpath = os.path.join(self.output_dir, "plot.csv")
            genremove(self.logger, plotpath)
            with open(plotpath, "wb") as plot_f:
                fieldnames = ["y_offset", "x_offset", "ch", "cs", "phase", "byte", "bit", "value"]
                writer = csv.DictWriter(plot_f, fieldnames=fieldnames, )
                writer.writeheader()
                for i in range(value):
                    row = {}
                    row["x_offset"] = DDILib.__convert_to_int(struct.unpack("<i", file.read(4))[0])
                    row["y_offset"] = DDILib.__convert_to_int(struct.unpack("<i", file.read(4))[0])
                    for ch in range(DDILib.NUM_CH):
                        row["ch"] = ch
                        for cs in range(DDILib.NUM_CS):
                            row["cs"] = cs
                            value = DDILib.__convert_to_int(struct.unpack("<I", file.read(4))[0])
                            for phase in range(DDILib.NUM_PHASE):
                                row["phase"] = phase
                                pv = value >> (16*phase)
                                for byte in range(DDILib.NUM_BYTE):
                                    row["byte"] = byte
                                    bv = pv >> (8*byte)

                                    override = False

                                    for bit in range(DDILib.NUM_BIT):
                                        row["bit"] = bit
                                        p = bv >> bit
                                        row["value"] = p & 1
                                        if override:
                                            row["value"] = row["value"] + 1000000
                                        writer.writerow(row)
            cmd_obj["PlotFile"] = plotpath

        elif (node.tag == "ParamArray" and name == "Frequency Settings"):
            if cmd_obj["DataPackets"] == None:
                cmd_obj["DataPackets"] = []

            packet = {}
            packet["DataPacketName"] = name
            packet["DataCount"] = value
            packet["DataPacketElements"] = []

            for i in range(value):
                freq_obj = {}               
                freq_obj["DataValues"] = []
                freq_obj["VoltageLevel"] = []
                nodelist = node.findall("./Attributes/Attribute")
                if(len(nodelist)>0):
                    for param in nodelist:
                        attrib_data = {}
                        self.__read_param(param, attrib_data, file)
                        freq_obj["DataValues"].append(attrib_data)
                else:
                    freq_obj["DataValues"] = None
                nodelist = node.findall("./Attributes/VoltageLevel")
                if(len(nodelist)>0):
                    for param in nodelist:
                        attrib_data = {}
                        self.__read_param(param, attrib_data, file)
                        freq_obj["VoltageLevel"].append(attrib_data)
                else:
                    freq_obj["VoltageLevel"] = None
                nodelist = node.findall("./Attributes/ParamArray")
                for param in nodelist:
                    freq_obj["DataPackets"] = None
                    self.__read_param(param, freq_obj, file)
                packet["DataPacketElements"].append(freq_obj)
            cmd_obj["DataPackets"].append(packet)
            

        elif node.tag == "ParamArray":
            if cmd_obj["DataPackets"] == None:
                cmd_obj["DataPackets"] = []
            self.logger.debug("find paramArray, name %s, value %d", name, value)
            packet = {}
            packet["DataPacketName"] = name
            packet["DataCount"] = value
            packet["DataPacketElements"] = []

            for i in range(value):
                for attrib in node.findall("./Attributes/Attribute"):
                    attrib_obj = {}
                    self.__read_param(attrib, attrib_obj, file)

                    nodelist = node.findall("./Attributes/Attribute/ParamArray")
                    for param in nodelist:
                        attrib_obj["DataPackets"] = None
                        self.__read_param(param, attrib_obj, file)

                    packet["DataPacketElements"].append(attrib_obj)

            cmd_obj["DataPackets"].append(packet)

        # handle attrib
        elif len(cmd_obj) == 0:
            cmd_obj["Name"] = name
            cmd_obj["Value"] = "0x%x" %(value)

        # handle normal cmd param
        else:
            if cmd_obj["CommandResults"] == None:
                cmd_obj["CommandResults"] = []
            cmd_obj["CommandResults"].append({"Name":name, "Value":value})


    def gen_ddi_output(self, ddi_output_bin, num_cmds):
        ddi_output = {}
        cmd_id_size = self.prot_root.find("./Commands/Command/Type").text
        ptypef = self.struct_type_conversion[cmd_id_size]
        cmd_id_size = self.int_to_size_conversion[cmd_id_size]
        self.logger.debug("Output Header Size: %d", cmd_id_size)

        with open(ddi_output_bin, "rb") as ddi_out_f:
            ddi_output["CommandsOutput"] = []
            for i in range(num_cmds):
                cmd_obj = {}
                cmd_id = struct.unpack("<"+ptypef, ddi_out_f.read(cmd_id_size))[0]
                cmd_obj["CommandID"] = DDILib.__convert_to_int(cmd_id)

                # setup default cmd_obj structure
                cmd_obj["CommandResults"] = None
                cmd_obj["DataPackets"] = None
                cmd_obj["PlotFile"] = None

                cmd_node = self.command_lookup[cmd_obj["CommandID"]]
                nodelist = cmd_node.findall("./Output/Params/*") + cmd_node.findall("./Output/ParamArray")
                for param in nodelist:
                    self.__read_param(param, cmd_obj, ddi_out_f)

                ddi_output["CommandsOutput"].append(cmd_obj)
        
        ddi_output_path = os.path.join(self.output_dir, "test_output.json")

        with open(ddi_output_path, "w") as ddi_out_f:
            ddi_out_f.write(json.dumps(ddi_output, indent=4))

        return ddi_output_path


    def decompose_xblconfig(self, xbl_config):

        if not self.xblconfig_tools_loaded:
            self.__get_xblconfig_tools()
        
        os.chdir(os.path.join(self.output_dir, "xblconfig"))
        cmd = "python GenXBLConfig.py -d %s -f ELF -o %s" %(xbl_config, os.path.join(self.output_dir, "decompose"))
        self.logger.debug(cmd)
        run_command(self.logger, cmd)
        return os.path.join(self.output_dir, "decompose")

    def __compose_xblconfig(self, name_tag=None):
        xbl_name = "xbl_config"

        if not self.xblconfig_tools_loaded:
            self.__get_xblconfig_tools()

        os.chdir(os.path.join(self.output_dir, "xblconfig"))
        input_json = os.path.join(self.output_dir, "decompose", "create_xbl_config.json")
        bin_path = os.path.join(self.output_dir, "decompose")

        if(name_tag is not None):
            xbl_name = xbl_name+name_tag
        output = os.path.join(self.output_dir, "compose", xbl_name)
        sectools_path = os.path.join(self.output_dir, "sectools")
        cmd = "python GenXBLConfig.py -i %s -b %s -f ELF -o %s --sectools_directory %s" %(input_json, bin_path, output, sectools_path)
        run_command(self.logger, cmd)

        xbl_name = xbl_name + ".elf"
        return os.path.join(self.output_dir, "compose", xbl_name)

    # TODO: remove this api
    def run_import_dsf(self, dram=None, dsf_lib=None):
        if self.import_dsf is None:
            if (dsf_lib):                
                dsf_script = os.path.join(dsf_lib, "ddr_fw", "host", "scripts")
                sys.path.append(dsf_script)
                if(dram=="LP5" or dram =="LP4"):
                    dsf_target_dir = os.path.join(dsf_lib, "release", "target", DDILibTargetSettings[self.target][dram])
            else:
                sys.path.append(os.path.join(self.boot_build, "boot_images", "QcomPkg", DDILibTargetSettings[self.target]["OSPathDSFPkg"], "Settings", "DSF", "release"))    

            from dsf import DSF
            if "SDM845" in self.target:
                dsf_target_dir +="_830"
            self.import_dsf = DSF(target_folder=dsf_target_dir)

        return

        
            
    def GenDcbBin(self, header_file, dcb_file, bin_files, output_path, output_file):
        import re
        binaries_ordered_by_header_index = []
        DCB_MAX_SIZE = 0
        
        define_re = re.compile(r'^#define[ \t]+([A-Z_]+)[ \t]+([\(\)\+\-\*\/0-9 xA-F]+)$')
        cfg_idx_re = re.compile(r'    ([0-9A-Z_]+)_CFG_IDX,$')
        idx_re = re.compile(r'    ([0-9A-Z_]+)_IDX,$')
        
        with open(header_file,"r") as dcb_header_fp:
            struct_flag = 0
            for line in dcb_header_fp:
                # Remove comments
                line = line.split("//")[0]
                
                # Check for a macro defined as a value or formula
                m = define_re.match(line)
                if m:
                    if m.group(1) == "DCB_MAX_SIZE":
                        DCB_MAX_SIZE = eval(m.group(2))
                    continue
                
                # Check for config index
                m = cfg_idx_re.match(line)
                if m:
                    binaries_ordered_by_header_index.append(m.group(1))
                    continue
                    
                    
                # Check for config index
                m = idx_re.match(line)
                if m:
                    if m.group(1) not in ["DCB_CRC", "DCB_SIZE", "DSF_VERSION"]:
                        binaries_ordered_by_header_index.append(m.group(1))
                    continue

        import struct
        dcbBinDict = {}
        checksum = 0
        dcb_size = 0
        version = 0
        with open(dcb_file, "rb") as dcb_fp:
            checksum = struct.unpack("<I", dcb_fp.read(4))[0]
            dcb_size = struct.unpack("<I", dcb_fp.read(4))[0]
            version = struct.unpack("<I", dcb_fp.read(4))[0]
            
            header_offset = 12
            for bin in binaries_ordered_by_header_index:
                bin_off = struct.unpack("<H", dcb_fp.read(2))[0]
                bin_size = struct.unpack("<H", dcb_fp.read(2))[0]
                header_offset += 4
                
                if bin in bin_files:
                    with open(bin_files[bin], "rb") as bin_f:
                        dcbBinDict[bin] = bin_f.read()
                else:
                    if bin_off != 0:
                        dcb_fp.seek(bin_off)
                        dcbBinDict[bin] = dcb_fp.read(bin_size)
                        dcb_fp.seek(header_offset)
        
        output = ""
        
        header_max_size = (len(binaries_ordered_by_header_index) + 3) * 4

        # Header bytes object
        header = bytes()

        # Data bytes object
        data = bytes()

        # Data starts at end of header
        offset = header_max_size

        # Loop through each binary section of DCB
        for bin in binaries_ordered_by_header_index:
            # Check if DCB sub-block exists
            if bin in dcbBinDict:
                # Update header
                size = len(dcbBinDict[bin])
                header += struct.pack("<H", offset)
                header += struct.pack("<H", size)
                offset += size

                # Update data
                data += dcbBinDict[bin]
                
                # Align each section of DCB to 8 byte boundary
                remainder = offset % 8
                if remainder != 0:
                    remainder = 8 - remainder
                for _ in range(remainder):
                    data += struct.pack("<B", 0)
                offset += remainder
            else:
                header += struct.pack("<H", 0)
                header += struct.pack("<H", 0)
                self.logger.warning("[WARNING] {:s} DCB sub-block not populated".format(bin))

        # DSF Version
        header = struct.pack("<I", version) + header

        # Size of DCB including header
        size = len(data) + header_max_size
        header = struct.pack("<I", size) + header

        # CRC set to zero as placeholder for CRC calculation
        header = struct.pack("<I",0) + header

        # Pad header with zeros
        header = header.ljust(header_max_size, b'\0')

        # Combine header and data
        binary = header + data

        # Pad binary with zeros
        binary = binary.ljust(DCB_MAX_SIZE, b'\0')

        # Calculate CRC on entire DCB binary
        CRC_POLYNOMIAL = 0x04C11DB7
        CRC_INITVAL = 0xFFFFFFFF
        CRC_XOROUT = 0xFFFFFFFF

        crc = CRC_INITVAL
        for octet in binary:
            if type(octet) is str:
                octet = struct.unpack("B", octet)[0]
            for bit in range(0,8):
                if (octet >> 7) ^ (crc >> 31):
                    crc = ((crc << 1) & 0xFFFFFFFF) ^ CRC_POLYNOMIAL
                else:
                    crc = (crc << 1) & 0xFFFFFFFF
                octet = (octet << 1) & 0xFF

        crc = crc ^ CRC_XOROUT

        # Replace placeholder CRC
        binary = struct.pack("<I", crc) + binary[4:]

        import os
        # Print information on which XL file is being processed and resulting binary
        # Print CRC and size information
        self.logger.info("[INFO] DCB {:s}, CRC 0x{:08X}, {:d}B free ({:d}/{:d} used)".format(
            os.path.basename(output_file),
            crc,
            DCB_MAX_SIZE - size,
            size,
            DCB_MAX_SIZE))

        # Write to file
        with open(os.path.join(output_path, output_file), "wb") as f:
            f.write(binary)

        
        
    def gen_xbl_config(self, dcb_input, ddi_input=None, ecdt_input=None, name_tag=None):
        # create new dcb based on old dcb, ddi input, and ecdt
        header_file =os.path.join(self.boot_build, "boot_images", "QcomPkg", DDILibTargetSettings[self.target]["OSPathDSFPkg"], "Library", "DSFTargetLib", "interface","header","api","ddr_shared_interface.h")

        binpaths = {}

        if ddi_input is not None:
            try:
                index = DDILib.BIN_NAMES.index(os.path.basename(ddi_input).strip(".bin"))
                binpaths[DDILib.BIN_NAMES[index]] = ddi_input
                self.logger.debug("Matched bin to DDILib BIN_NAMES name:%s path:%s", DDILib.BIN_NAMES[index], ddi_input)
            except ValueError:
                self.logger.debug("Could not match DDILib BIN_NAMES with: "+ddi_input)
                sys.exit()

        if ecdt_input is not None:
            for bin in ecdt_input:
                try:
                    cur_binname = os.path.basename(bin)
                    for name_check in DDILib.BIN_NAMES:
                        if name_check in cur_binname:
                            cur_binname = name_check
                            break
                    index = DDILib.BIN_NAMES.index(cur_binname)
                    binpaths[DDILib.BIN_NAMES[index]] = bin
                    self.logger.debug("Matched bin to DDILib BIN_NAMES name:%s path:%s", DDILib.BIN_NAMES[index], bin)
                except ValueError:
                    self.logger.debug("Could not match DDILib binname with: "+bin)
                    sys.exit()

                    
        for dcb_bin in glob.glob(dcb_input + '/*_dcb.bin'):
            #self.import_dsf.PopulateDcbBinDictFromExt(dcb_bin, binpaths)
            # save off the input DCB for debug, new DCB will be generated in its place
            if (os.path.exists(dcb_input)):
                original_dcb = os.path.join(dcb_input, "orig_"+os.path.basename(dcb_bin))
                shutil.move(dcb_bin,original_dcb)
                self.GenDcbBin(header_file, original_dcb, binpaths, os.path.join(self.output_dir, "decompose"), dcb_bin)
                #self.import_dsf.GenerateDcbBinary(os.path.basename(dcb_bin), populate_first=False, output=os.path.join(self.output_dir, "decompose"))

        if(name_tag is not None):
            return self.__compose_xblconfig(name_tag)
        return self.__compose_xblconfig()


    def update_dcb_bin(self, dcb_path, ddi_input=None, ecdt_input=None):
        # create new dcb based on old dcb, ddi input, and ecdt
        header_file =os.path.join(self.boot_build, "boot_images", "QcomPkg", DDILibTargetSettings[self.target]["OSPathDSFPkg"], "Library", "DSFTargetLib", "interface","header","api","ddr_shared_interface.h")

        binpaths = {}

        if ddi_input is not None:
            try:
                index = DDILib.BIN_NAMES.index(os.path.basename(ddi_input).strip(".bin"))
                binpaths[DDILib.BIN_NAMES[index]] = ddi_input
                self.logger.debug("Matched bin to DDILib BIN_NAMES name:%s path:%s", DDILib.BIN_NAMES[index], ddi_input)
            except ValueError:
                self.logger.debug("Could not match DDILib BIN_NAMES with: "+ddi_input)
                sys.exit()

        if ecdt_input is not None:
            for bin in ecdt_input:
                try:
                    cur_binname = os.path.basename(bin)
                    for name_check in DDILib.BIN_NAMES:
                        if name_check in cur_binname:
                            cur_binname = name_check
                            break
                    index = DDILib.BIN_NAMES.index(cur_binname)
                    binpaths[DDILib.BIN_NAMES[index]] = bin
                    self.logger.debug("Matched bin to DDILib BIN_NAMES name:%s path:%s", DDILib.BIN_NAMES[index], bin)
                except ValueError:
                    self.logger.debug("Could not match DDILib binname with: "+bin)
                    sys.exit()

                    
        for dcb_bin in glob.glob(dcb_path + '/*_dcb.bin'):
            #self.import_dsf.PopulateDcbBinDictFromExt(dcb_bin, binpaths)
            # save off the input DCB for debug, new DCB will be generated in its place
            if (os.path.exists(dcb_path)):
                original_dcb = os.path.join(dcb_path, "orig_"+os.path.basename(dcb_bin))
                shutil.move(dcb_bin,original_dcb)
                self.GenDcbBin(header_file, original_dcb, binpaths, os.path.join(self.output_dir, "decompose"), dcb_bin)
                #self.import_dsf.GenerateDcbBinary(os.path.basename(dcb_bin), populate_first=False, output=os.path.join(self.output_dir, "decompose"))


def main():
    parser = argparse.ArgumentParser(description="DDI helper library")
    parser.add_argument("-p", "--prot_xml", help="path to DDI protocol XML file")
    parser.add_argument("-b", "--boot_build", help="path to boot CRM build base")
    parser.add_argument("-o", "--output_dir", default=tempfile.mkdtemp(prefix="ddi_"), help="output location to store debug logs and output files. If not supplied, a OS temporary directory will be created")
    parser.add_argument("-r", "--run_id", default="run_output", help="run_id, timestamp of current run")    
    parser.add_argument("-v", "--verbose", default="INFO", choices=["DEBUG", "INFO", "WARNING" ,"ERROR"], help="logging verbosity to be send to the cmd window. By default, all debug logs will be sent to a file")
    parser.add_argument("-a","--target", help="target device")
    parser.add_argument("-c","--flash", help="device flash type")

    subparsers = parser.add_subparsers(help="sub-command help")

    parser_input = subparsers.add_parser("in", help="command for flashing tests/eCDT to the device. This will generate a xbl_config.elf file.")
    parser_input.add_argument("-t", "--test_config", help="path to DDI test case config")
    parser_input.add_argument("-e", "--ecdt_bins", metavar="BIN", nargs=2, help="list of ecdt binary paths")
    parser_input.add_argument("-x", "--xbl_config", required=True, help="path to xbl_config.elf file")
    parser_input.set_defaults(which="in")

    parser_output = subparsers.add_parser("out", help="command for reading output from the device. This will decompose the DDR flash partition.")
    parser_output.add_argument("-d", "--ddr_bin", required=True, help="path to DDR flash partition binary")
    parser_output.add_argument("-n", "--num_cmds", type=int, required=True, help="number of commands to get output ddr binary") # TODO remove
    parser_output.set_defaults(which="out")

    parser_write = subparsers.add_parser("write", help="command for writing to the device's flash partition")
    parser_write.add_argument("-f", "--file", required=True, help="path to binary file which will be flashed to the device")
    parser_write.add_argument("-p", "--partition", required=True, help="partition to use. (ie. ddr, xbl_config)")
    parser_write.add_argument("-l", "--dp_lite", required=True, help="path to the device programmer lite file to use")
    group = parser_write.add_mutually_exclusive_group(required=True)
    group.add_argument("-m", "--meta", help="meta build used for this device")
    group.add_argument("-x", "--partition_xmls", metavar="XML", nargs="+", help="list of xml files used for flashing the partition. This is a combination of partition.xml and rawprogramX.xml files.")
    parser_write.add_argument("--port", help="EDL port number. If not supplied the port will be auto detected")
    parser_write.set_defaults(which="write")

    parser_full = subparsers.add_parser("full", help="running a full DDI test. Flash test, run test, and read output")
    parser_full.add_argument("-l", "--dp_lite", required=True, help="path to the device programmer lite file to use")
    group = parser_full.add_mutually_exclusive_group(required=True)
    group.add_argument("-m", "--meta", help="meta build used for this device")
    group.add_argument("-x", "--partition_xmls", metavar="XML", nargs="+", help="list of xml files used for flashing the partition. This is a combination of partition.xml and rawprogramX.xml files.")
    parser_full.add_argument("-t", "--test_config", help="path to DDI test case config")
    parser_full.add_argument("-e", "--ecdt_bins", metavar="BIN", nargs=2, help="list of ecdt binary paths")
    parser_full.add_argument("--port", help="EDL port number. If not supplied the port will be auto detected")
    parser_full.add_argument("--retrain", action="store_true", help="Flag to trigger retraining of the DDR partition")
    parser_full.set_defaults(which="full")

    parser_update = subparsers.add_parser("update", help="command for updating the xbl_config partition in the device to inlcude the eCDT_PRE and eCDT_POST bins.")
    parser_update.add_argument("-l", "--dp_lite", required=True, help="path to the device programmer lite file to use")
    group = parser_update.add_mutually_exclusive_group(required=True)
    group.add_argument("-m", "--meta", help="meta build used for this device")
    group.add_argument("-x", "--partition_xmls", metavar="XML", nargs="+", help="list of xml files used for flashing the partition. This is a combination of partition.xml and rawprogramX.xml files.")
    parser_update.add_argument("-e", "--ecdt_bins", metavar="BIN", nargs=2, help="list of ecdt binary paths")
    parser_update.add_argument("-r", "--port", help="EDL port number. If not supplied the port will be auto detected")
    parser_update.add_argument("--retrain", action="store_true", help="Flag to trigger retraining of the DDR partition")
    parser_update.set_defaults(which="update")

    args = parser.parse_args()

    if not args.output_dir is None and not os.path.isdir(args.output_dir):
        os.makedirs(args.output_dir)
    
    run_output = os.path.join(args.output_dir, args.run_id)
    if not os.path.isdir(run_output):
        os.makedirs(run_output)
    init_time = int(time.time())
    ddi_log = "ddi_debug_log_"+str(init_time)+".txt"
    logging.basicConfig(filename=os.path.join(run_output, ddi_log), level=logging.getLevelName(args.verbose), filemode="w")
    rootlogger = logging.getLogger()
    ch = logging.StreamHandler()
    ch.setLevel(logging.getLevelName(args.verbose))
    rootlogger.addHandler(ch)
    logger = logging.getLogger("ddi")

    if args.which == "in" or args.which == "out" or args.which == "full" or args.which == "update":
        ddilib = DDILib(args.output_dir, args.prot_xml, boot_build=args.boot_build, target=args.target)
    if args.which == "write" or args.which == "full" or args.which == "update":
        flashinterface = FlashInterface(args.output_dir, args.meta, boot_build=args.boot_build, partition_xmls=args.partition_xmls, flash_type=args.flash)

    # Read XBL_config from device
    if args.which == "in":
        if args.test_config is not None:
            ddi_in_bin = ddilib.gen_ddi_input(args.test_config)
        else:
            ddi_in_bin = None

        # Generate xbl_config.elf
        if fnmatch.fnmatch(os.path.basename(args.xbl_config), "xbl_config.elf"):
            decompose_path = ddilib.decompose_xblconfig(args.xbl_config)
                  
            xbl_config = ddilib.gen_xbl_config(decompose_path, ddi_input=ddi_in_bin, ecdt_input=args.ecdt_bins)
            logger.info("")
            logger.info("xbl_config generated at: %s", xbl_config)
        # Generate DCB binary
        elif os.path.isdir(args.xbl_config):
            ddilib.update_dcb_bin(args.xbl_config, ddi_input=ddi_in_bin, ecdt_input=args.ecdt_bins)
            num_dcb_processed=0
            for dcb_bin in glob.glob(args.xbl_config + '/orig_*_dcb.bin'):
                if os.path.exists(os.path.dirname(dcb_bin)+ os.path.basename(dcb_bin).replace('orig_','/')):
                    num_dcb_processed = num_dcb_processed + 1
                    logger.info("DCB %s is processed", dcb_bin)
                else:
                    logger.info("DCB %s is not processed", dcb_bin)
            logger.info("%d DCB generated at: %s", num_dcb_processed, args.xbl_config)
        else:
            logger.info("")
            logger.info("Unrecognized input: %s", args.xbl_config)

    # Read ddi_output from device
    if args.which == "out":
        ddi_output = ddilib.get_ddi_output_bin(args.ddr_bin)
        test_output = ddilib.gen_ddi_output(ddi_output, args.num_cmds)
        logger.info("test_output.json generated at: %s", test_output)

    # Read particular memory partition from device.
    if args.which == "write":
        flashinterface.load_dp_lite(args.dp_lite)
        flashinterface.write_partition(args.partition, args.file)
        logger.info("flashed %s to partition %s", args.file, args.partition)

    if args.which == "update":
        logger.info("")
        logger.info("----------  Start DDI command")
        logger.info("+ command     :  update")
        logger.info("+ meta        : %s", args.meta)
        logger.info("+ device prog : %s", args.dp_lite)
        logger.info("+ eCDT binary : %s", args.ecdt_bins)
        logger.info("+ eDL port    : COM%s", args.port)
        logger.info("+")
        logger.info("------------------------------------")
        logger.info("")
        logger.info("----------  Prepare for Connecting Device")

        local_edl_port = flashinterface.get_edl_port_num()
        if(local_edl_port == -1):
           sys.exit()

        if (local_edl_port != args.port):
            args.port = local_edl_port
            logger.warning("[WARNING] different EDL port detected")
            
        flashinterface.load_dp_lite(args.dp_lite)

        logger.info("")
        logger.info("----------  Read XBL_config partition")
        # read gpt from device
        orig_xbl_config = flashinterface.read_partition("partition_table")
        # read original xbl_config partition from device
        orig_xbl_config = flashinterface.read_partition("xbl_config")
        logger.info("original partition xbl_config saved at: %s", orig_xbl_config)

        # create new xbl_config
        ddi_in_bin = None
        decompose_path = ddilib.decompose_xblconfig(orig_xbl_config)

        logger.info("")
        logger.info("----------  Write XBL_config partition")
        
        new_xbl_config = ddilib.gen_xbl_config(decompose_path, ddi_input=ddi_in_bin, ecdt_input=args.ecdt_bins)
        logger.info("new xbl_config generated at: %s", new_xbl_config)
        flashinterface.write_partition("xbl_config", new_xbl_config)
        logger.info("new xbl_config flashed to device")

        logger.info("")
        logger.info("----------  Update DDI input")
        
        # read ddr flash partition
        orig_ddr_partition = flashinterface.read_partition("ddr")
        logger.info("original partition ddr saved at: %s", orig_ddr_partition)

        # modify ddr flash partition to clear out DDI_OUTPUT section
        # optionally clear out entire partition for eye plot commands
        new_ddr_partition = ddilib.clear_ddi_output_bin(orig_ddr_partition, clear_all=args.retrain)

        # flash edited ddr flash partition
        flashinterface.write_partition("ddr", new_ddr_partition)
        logger.info("new ddr partition flashed to device")


    if args.which == "full":

        logger.info("")
        logger.info("----------  Start DDI command")
        logger.info("+ command     : Run DDI command")
        logger.info("+ test config : %s", args.test_config)
        logger.info("+ ddi protocol: %s", args.prot_xml)
        logger.info("+ meta        : %s", args.meta)
        logger.info("+ device prog : %s", args.dp_lite)
        logger.info("+ eDL port    : COM%s", args.port)
        logger.info("+")
        logger.info("-------------------------------------")
        logger.info("")
        logger.info("----------  Prepare for Connecting Device")

        os.chdir(args.output_dir)
        logger.info("current working dir: %s:", os.getcwd())
        
        local_edl_port = flashinterface.get_edl_port_num()
        if(local_edl_port == -1):
           sys.exit()

        if (local_edl_port != args.port):
            args.port = local_edl_port
            logger.warning("[WARNING] different EDL port detected")
            
        flashinterface.load_dp_lite(args.dp_lite)


        logger.info("")
        logger.info("----------  Read XBL_config partition")

        # read gpt from device
        orig_xbl_config = flashinterface.read_partition("partition_table")
        logger.info("original partition xbl_config saved at: %s", orig_xbl_config)

        # read original xbl_config partition from device
        orig_xbl_config = flashinterface.read_partition("xbl_config")
        logger.info("original partition xbl_config saved at: %s", orig_xbl_config)


        logger.info("")
        logger.info("----------  Create new XBL_config partition")

        # create new xbl_config
        if args.test_config is not None:
            ddi_in_bin = ddilib.gen_ddi_input(args.test_config)
        else:
            ddi_in_bin = None

        decompose_path = ddilib.decompose_xblconfig(orig_xbl_config)
      
        new_xbl_config = ddilib.gen_xbl_config(decompose_path, ddi_input=ddi_in_bin, ecdt_input=args.ecdt_bins)
        logger.info("new xbl_config generated at: %s", new_xbl_config)

        # flash xbl_config
        flashinterface.write_partition("xbl_config", new_xbl_config)
        logger.info("new xbl_config flashed to device")

        logger.info("")
        logger.info("----------  Update DDR partition and clear DDI area")

        # read ddr flash partition
        orig_ddr_partition = flashinterface.read_partition("ddr")
        logger.info("original partition ddr saved at: %s", orig_ddr_partition)

        # modify ddr flash partition to clear out DDI_OUTPUT section
        # optionally clear out entire partition for eye plot commands
        new_ddr_partition = ddilib.clear_ddi_output_bin(orig_ddr_partition, clear_all=args.retrain)

        # flash edited ddr flash partition
        flashinterface.write_partition("ddr", new_ddr_partition)
        logger.info("new ddr partition flashed to device")


        logger.info("")
        logger.info("----------  Reset Device")

        # prompt for device reboot
        flashinterface.reset_device()
        
        if(flashinterface.get_edl_port_num() == -1):
            sys.exit()

        logger.info("")
        logger.info("----------  Setup Sahara mode")

        # trasition device from EDL to Sahara mode
        flashinterface.load_dp_lite(args.dp_lite)


        logger.info("")
        logger.info("----------  Read DDI command result")

        # read ddr
        ddr_partition = flashinterface.read_partition("ddr")
        logger.info("read ddr partition from device at %s", ddr_partition)

        # flash original xbl_config to clear ddi input from the current run
        flashinterface.write_partition("xbl_config", orig_xbl_config)
        logger.info("new xbl_config flashed to device")

        # get test output from ddr
        ddi_output = ddilib.get_ddi_output_bin(ddr_partition)
        num_cmds = 0
        with open(args.test_config) as f_tc:
            tc_dict = json.load(f_tc)
            num_cmds = len(tc_dict["Commands"])

        test_output = ddilib.gen_ddi_output(ddi_output, num_cmds)
        logger.info("test_output.json generated at: %s", test_output)

    logger.info("")
    logger.debug("end of script: argv: %s" + str(sys.argv))
    logger.info("output generated at: %s", run_output)
    logger.info("----------  Finish DDI command")    

if __name__ == "__main__":
    main()
