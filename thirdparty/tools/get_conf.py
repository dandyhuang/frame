import os
import sys
import logging
import socket
from pyhocon import ConfigFactory
from pyhocon import config_tree
import subprocess

# import path
root = '.'
sys.path.append(os.path.join(root, ''))

# logging.
root_path = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
logger = logging.getLogger()


deploy_path = root_path + '/deploy/'
conf_path = root_path + '/zeus_conf/'
exp_path = sys.argv[1]  if(len(sys.argv) == 2)  else  ""
# global param


host_ip = socket.gethostbyname(socket.gethostname())


def get_tag_conf(ip_tag, cf_tag):
    if host_ip in ip_tag:
        tags = []
        for tag in cf_tag:
            tags.append(tag)
        for tag in ip_tag[host_ip]:
            tags.remove(tag)
        print tags
        for tag in tags:
            if  os.path.isdir(tag):
                res = str(subprocess.check_output("rm -r  " + tag, shell=True))
                print(res)
    else:
        for tag in cf_tag:
            for path in cf_tag[tag]:
                full_path = conf_path + cf_tag[tag][path]
                if path == "conf":
                    res = str(subprocess.check_output("cp -r  " + full_path + " " + deploy_path + tag, shell=True))
                else:
                    res = str(subprocess.check_output("cp -r  " + full_path + " " + deploy_path + path, shell=True))
                print(res)


def get_handler_cf():
    ip_conf =  conf_path + exp_path + "/ip_list.conf"
    tag_conf = conf_path + exp_path + "/tag_list.conf"
    if (not os.path.isfile(ip_conf)) | (not os.path.isfile(tag_conf)):
        ip_conf = "tools/ip_list.conf"
        tag_conf = "tools/tag_list.conf"
    ip_handler = ConfigFactory.parse_file(ip_conf)
    tag_handler = ConfigFactory.parse_file(tag_conf)
    get_tag_conf(ip_handler,tag_handler)


def main_handler():
    get_handler_cf()


if __name__ == '__main__':
    main_handler()
