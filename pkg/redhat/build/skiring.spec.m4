Name: skiring
Version: VERSION
Release: RELEASE
Summary: A simple keychain tool for scripts
URL: https://github.com/dboitnot/skiring
License: BSD-3
Group: Applications/System
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Buildarch: x86_64

%description
A simple keyring for your shell.

%prep
%setup -q
%build
%configure skiring_user=root
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(0755,root,root,-)
/usr/bin/dbpw
/usr/bin/skiring

%post
useradd -M -s /bin/bash skiring || echo "skiring user already exists"
chown skiring /usr/bin/skiring
chmod 4755 /usr/bin/skiring
mkdir -p /etc/skiring
chown skiring /etc/skiring
chmod 700 /etc/skiring
[ -f /etc/skiring/ring ] || /usr/bin/skiring init
chmod 600 /etc/skiring/ring
