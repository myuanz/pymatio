FROM quay.io/pypa/manylinux2014_x86_64 as builder

RUN curl -fsSL https://xmake.io/shget.text | bash

FROM quay.io/pypa/manylinux2014_x86_64

COPY --from=builder /root/.xmake/ /root/.xmake/
COPY --from=builder /root/.local/ /root/.local/


RUN echo '#!/bin/bash' > /entrypoint.sh && \
    echo "export XMAKE_ROOT=y" >> /entrypoint.sh && \
    echo 'source /root/.xmake/profile' >> /entrypoint.sh && \
    echo 'exec "$@"' >> /entrypoint.sh && \
    chmod +x /entrypoint.sh && \
    source /entrypoint.sh && \
    xrepo install -y --build zlib hdf5

COPY --from=builder /root/.xmake/ /root/.xmake/

ENTRYPOINT ["/entrypoint.sh"]