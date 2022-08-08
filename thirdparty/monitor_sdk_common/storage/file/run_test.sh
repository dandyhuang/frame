#!/usr/bin/env bash

JAVA_HOME=/home/users/liubin18/storm/software/jdk1.6.0_24
HADOOP_HOME=/home/users/liubin18/storm/software/hadoop-2.2.0/share/hadoop

JAVA_JRE=$JAVA_HOME/jre
JAVA_BIN=$JAVA_HOME/bin
PATH=$JAVA_BIN:$PATH
CLASSPATH=.:$JAVA_HOME/lib/dt.jar:$JAVA_HOME/lib/tools.jar
LIBRARY_PATH=$LIBRARY_PATH:$JAVA_JRE/lib/amd64/server/
LD_LIBRARY_PATH=$LIBRARY_PATH:$LD_LIBRARY_PATH

# add jar in common
if [ -d "$HADOOP_HOME/common" ]; then
  CLASSPATH=${CLASSPATH}:$HADOOP_HOME/common
fi

for f in $HADOOP_HOME/common/*.jar; do
  CLASSPATH=${CLASSPATH}:$f;
done

for f in $HADOOP_HOME/common/lib/*.jar; do
  CLASSPATH=${CLASSPATH}:$f;
done

# add jar in hdfs
if [ -d "$HADOOP_HOME/hdfs" ]; then
  CLASSPATH=${CLASSPATH}:$HADOOP_HOME/hdfs
fi

for f in $HADOOP_HOME/hdfs/*.jar; do
  CLASSPATH=${CLASSPATH}:$f;
done

for f in $HADOOP_HOME/hdfs/lib/*.jar; do
  CLASSPATH=${CLASSPATH}:$f;
done

# add user-specified CLASSPATH last
if [ "$HADOOP_CLASSPATH" != "" ]; then
  CLASSPATH=${CLASSPATH}:${HADOOP_CLASSPATH}
fi

export CLASSPATH
export LD_LIBRARY_PATH

#./hdfs_test --v=1
blade test -pdebug ... --testarg='--gtest_also_run_disabled_tests'
