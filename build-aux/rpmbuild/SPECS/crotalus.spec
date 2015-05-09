Name:           crotalus
Version:        2.26
Release:        1%{?dist}
Summary:        Crotalus Web Daemon

Group:          System Environment/Daemons
License:        GPLv3+
URL:            http://www.sharelib.net/
Source0:        http://www.sharelib.net/release/crotalus-2.26.tar.gz

BuildRequires:  libshare-devel
Requires:       libshare

%description




%prep
%setup -q


%build
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
find $RPM_BUILD_ROOT -name '*.la' -exec rm -f {} ';'


%check
make check


%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%{_sbindir}/crotalus
%{_infodir}/crotalus.info.gz
%{_docdir}/crotalus/crotalus_html.zip
%{_docdir}/dir

%changelog
* Fri May  9 2015 Neo Natura <support@neo-natura.com> - 2.26
- Initial RPM release version of this package.
