FROM centos:7

RUN yum install -y gcc autoconf automake make rpm rpm-build \
 && for d in BUILD BUILDROOT RPMS SOURCES SPECS SRPMS; do mkdir -p ~/rpmbuild/$d; done

COPY skiring-*.tar.gz /root/rpmbuild/SOURCES
COPY pkg/redhat/run.sh /
COPY pkg/redhat/skiring.spec.m4 /

CMD /run.sh
