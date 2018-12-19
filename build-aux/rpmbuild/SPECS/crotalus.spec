Name:           crotalus
Version:        3.02
Release:        1%{?dist}
Summary:        Crotalus Web Daemon

Group:          System Environment/Daemons
License:        GPLv3+
URL:            http://www.sharelib.net/
Source0:        http://www.sharelib.net/release/crotalus-3.02.tar.gz

Requires:       libshare
Requires(post): info
Requires(preun): info

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
/sbin/install-info %{_infodir}/%{name}.info %{_infodir}/dir || :

%postun -p /sbin/ldconfig
if [ $1 = 0 ] ; then
  /sbin/install-info --delete %{_infodir}/%{name}.info %{_infodir}/dir || :
fi

%files
%{_sbindir}/crotalus
%{_bindir}/crphp
%{_infodir}/crotalus.info.gz
%{_infodir}/dir
%{_docdir}/crotalus/crotalus_html.zip

%changelog
* Wed Dec 12 2018 Neo Natura <support@neo-natura.com> - 3.02
- The RPM release version of this package.
